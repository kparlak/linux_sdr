----------------------------------------------------------------------------------
-- Company: EN.525.742 - System-on-a-Chip FPGA Design Laboratory
-- Engineer: Kevin Parlak
-- 
-- Create Date: 09/09/2022 06:51:49 AM
-- Design Name: Audio Player
-- Module Name: low_level_dac_if - Behavioral
-- Project Name: 
-- Target Devices: Zybo Z7-20 - SSM2603 DAC
-- Tool Versions: 
-- Description: Generates appropriate signals for the SSM2603 DAC
-- 
-- Dependencies: clkdivider.vhd
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments: Derived from class provided files
-- 
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity lowlevel_dac_if is
generic (channels : in integer := 2);
port (
    rst          : in std_logic; -- Active high asynchronous reset
    clk125       : in std_logic; -- Master clock for all flip-flops
    data_word    : in std_logic_vector(channels*16-1 downto 0); -- 16 or 32 bit input word
    sdata        : out std_logic; -- Serial data out to the DAC
    lrck         : out std_logic; -- 48.828125 khz clock representing 16 bits
    bclk         : out std_logic; -- 1.5625 mhz DAC clock
    mclk         : out std_logic; -- 12.5 mhz clock
    latched_data : out std_logic; -- 1 clk125 wide pulse indicating changing of data_word is valid
    valid        : in std_logic -- Ignore, used for AXI interface
);
end lowlevel_dac_if;

architecture Behavioral of lowlevel_dac_if is

    signal toggle_mclk : std_logic := '0';
    signal tmp_mclk : std_Logic := '0';
    signal toggle_bclk : std_logic := '0';
    signal tmp_bclk : std_logic := '0';
    signal bclk_falling_edge : std_logic := '0';
    signal bit_cnt : unsigned(4 downto 0) := (others => '0');
    signal shift_reg : unsigned(31 downto 0) := (others => '0');

begin

    -- Generate mclk
    make_mclk: entity work.clkdivider generic map (divideby => 5)
        port map (rst => rst, clk => clk125, pulse_out => toggle_mclk);

    -- Generate bclk
    make_bclk : entity work.clkdivider generic map (divideby => 40)
        port map (rst => rst, clk => clk125, pulse_out => toggle_bclk);

    dac_clks : process(clk125, rst)
    begin
        if(rst = '1') then
            tmp_mclk <= '0';
            tmp_bclk <= '0';
        elsif(rising_edge(clk125)) then
            if(toggle_mclk = '1') then
                tmp_mclk <= not tmp_mclk;
            end if;
            if(toggle_bclk = '1') then
                tmp_bclk <= not tmp_bclk;
            end if;
        end if;
    end process dac_clks;

    -- Generate a falling edge trigger for shifting of data
    bclk_falling_edge <= toggle_bclk and tmp_bclk;

    -- Shift data out MSB to LSB
    shifter : process(clk125, rst)
    begin
        if(rst = '1') then
            latched_data <= '0';
            bit_cnt <= (others => '0');
            lrck <= '0';
            shift_reg <= (others => '0');
        elsif(rising_edge(clk125)) then
            latched_data <= '0';
            if(bclk_falling_edge = '1') then
                bit_cnt <= bit_cnt + 1;
                shift_reg <= shift_left(shift_reg, 1);
                -- lrck is low 16-31
                if(bit_cnt = 0) then
                    lrck <= '0';
                elsif(bit_cnt = 1) then
                    if(channels = 2)then
                        shift_reg <= unsigned(data_word);
                    else
                        shift_reg <= unsigned(data_word & data_word);
                    end if; 
                    latched_data <= '1';
                -- lrck is high 0-15
                elsif(bit_cnt = 16) then
                    lrck <= '1';
                end if;
            end if;
        end if;
    end process shifter;

    sdata <= shift_reg(31);
    mclk <= tmp_mclk;
    bclk <= tmp_bclk;

end Behavioral;
