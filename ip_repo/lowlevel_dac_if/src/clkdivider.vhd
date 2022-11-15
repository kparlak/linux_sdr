----------------------------------------------------------------------------------
-- Company: EN.525.742 - System-on-a-Chip FPGA Design Laboratory
-- Engineer: Kevin Parlak
-- 
-- Create Date: 09/09/2022 06:51:49 AM
-- Design Name: Audio Player
-- Module Name: clkdivider - Behavioral
-- Project Name: 
-- Target Devices: Generic
-- Tool Versions: 
-- Description: Generates a pulse based on an input clock and divisor
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments: Derived from class provided files
-- 
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity clkdivider is
    generic (divideby : natural := 2);
    port (
        rst       : in std_logic; -- Active high asynchronous reset
        clk       : in std_logic; -- Input clock
        pulse_out : out std_logic -- Output pulse
    );
end clkdivider;

architecture Behavioral of clkdivider is

    signal cnt : natural range 0 to divideby - 1;

begin

    process(clk, rst)
    begin
        if(rst = '1') then
            cnt <= 0;
        elsif rising_edge(clk) then
            if(cnt = divideby - 1) then
                cnt <= 0;
            else
                cnt <= cnt + 1;
            end if;
        end if;
    end process;

    pulse_out <= '1' when cnt = divideby - 1 else '0';

end Behavioral;
