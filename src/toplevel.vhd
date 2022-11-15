----------------------------------------------------------------------------------
-- Company: EN.525.742 - System-on-a-Chip FPGA Design Laboratory
-- Engineer: Kevin Parlak
-- 
-- Create Date: 11/15/2022 08:14:53 AM
-- Design Name: Linux SDR
-- Module Name: toplevel - Behavioral
-- Project Name: 
-- Target Devices: Zybo Z7-20
-- Tool Versions: 
-- Description: Instantiates top level design
-- 
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;

entity toplevel is
    port (
    -- DOES NOT NEED CONSTRAINTS
        DDR_addr : inout STD_LOGIC_VECTOR(14 downto 0);
        DDR_ba : inout STD_LOGIC_VECTOR(2 downto 0);
        DDR_cas_n : inout STD_LOGIC;
        DDR_ck_n : inout STD_LOGIC;
        DDR_ck_p : inout STD_LOGIC;
        DDR_cke : inout STD_LOGIC;
        DDR_cs_n : inout STD_LOGIC;
        DDR_dm : inout STD_LOGIC_VECTOR(3 downto 0);
        DDR_dq : inout STD_LOGIC_VECTOR(31 downto 0);
        DDR_dqs_n : inout STD_LOGIC_VECTOR(3 downto 0);
        DDR_dqs_p : inout STD_LOGIC_VECTOR(3 downto 0);
        DDR_odt : inout STD_LOGIC;
        DDR_ras_n : inout STD_LOGIC;
        DDR_reset_n : inout STD_LOGIC;
        DDR_we_n : inout STD_LOGIC;
        FIXED_IO_ddr_vrn : inout STD_LOGIC;
        FIXED_IO_ddr_vrp : inout STD_LOGIC;
        FIXED_IO_mio : inout STD_LOGIC_VECTOR(53 downto 0);
        FIXED_IO_ps_clk : inout STD_LOGIC;
        FIXED_IO_ps_porb : inout STD_LOGIC;
        FIXED_IO_ps_srstb : inout STD_LOGIC;
    -- NEEDS CONSTRAINTS
        ac_pblrc : out std_logic; -- lrck (48.828125 khz clock representing 16 bits)
        ac_pbdat : out std_logic; -- sdata (Serial data out to the DAC)
        ac_bclk  : out std_logic; -- bclk (1.5625 mhz DAC clock)
        ac_mclk  : out std_logic; -- mclk (12.5 mhz clock)
        ac_muten : out std_logic;
        ac_scl : inout STD_LOGIC;
        ac_sda : inout STD_LOGIC
    );
end toplevel;

architecture Behavioral of toplevel is

    component system is
    port (
        DDR_cas_n : inout STD_LOGIC;
        DDR_cke : inout STD_LOGIC;
        DDR_ck_n : inout STD_LOGIC;
        DDR_ck_p : inout STD_LOGIC;
        DDR_cs_n : inout STD_LOGIC;
        DDR_reset_n : inout STD_LOGIC;
        DDR_odt : inout STD_LOGIC;
        DDR_ras_n : inout STD_LOGIC;
        DDR_we_n : inout STD_LOGIC;
        DDR_ba : inout STD_LOGIC_VECTOR(2 downto 0);
        DDR_addr : inout STD_LOGIC_VECTOR(14 downto 0);
        DDR_dm : inout STD_LOGIC_VECTOR(3 downto 0);
        DDR_dq : inout STD_LOGIC_VECTOR(31 downto 0);
        DDR_dqs_n : inout STD_LOGIC_VECTOR(3 downto 0);
        DDR_dqs_p : inout STD_LOGIC_VECTOR(3 downto 0);
        FIXED_IO_mio : inout STD_LOGIC_VECTOR(53 downto 0);
        FIXED_IO_ddr_vrn : inout STD_LOGIC;
        FIXED_IO_ddr_vrp : inout STD_LOGIC;
        FIXED_IO_ps_srstb : inout STD_LOGIC;
        FIXED_IO_ps_clk : inout STD_LOGIC;
        FIXED_IO_ps_porb : inout STD_LOGIC;
        iic_rtl_scl_i : in STD_LOGIC;
        iic_rtl_scl_o : out STD_LOGIC;
        iic_rtl_scl_t : out STD_LOGIC;
        iic_rtl_sda_i : in STD_LOGIC;
        iic_rtl_sda_o : out STD_LOGIC;
        iic_rtl_sda_t : out STD_LOGIC;
        sdata : out STD_LOGIC;
        lrck : out STD_LOGIC;
        bclk : out STD_LOGIC;
        mclk : out STD_LOGIC
    );
    end component system;

    component IOBUF is
    port (
        I : in STD_LOGIC;
        O : out STD_LOGIC;
        T : in STD_LOGIC;
        IO : inout STD_LOGIC
    );
    end component IOBUF;

    signal iic_rtl_scl_i : STD_LOGIC;
    signal iic_rtl_scl_o : STD_LOGIC;
    signal iic_rtl_scl_t : STD_LOGIC;
    signal iic_rtl_sda_i : STD_LOGIC;
    signal iic_rtl_sda_o : STD_LOGIC;
    signal iic_rtl_sda_t : STD_LOGIC;

begin

    -- Disable mute
    ac_muten <= '1';

    system_inst: component system
    port map (
        DDR_addr(14 downto 0) => DDR_addr(14 downto 0),
        DDR_ba(2 downto 0) => DDR_ba(2 downto 0),
        DDR_cas_n => DDR_cas_n,
        DDR_ck_n => DDR_ck_n,
        DDR_ck_p => DDR_ck_p,
        DDR_cke => DDR_cke,
        DDR_cs_n => DDR_cs_n,
        DDR_dm(3 downto 0) => DDR_dm(3 downto 0),
        DDR_dq(31 downto 0) => DDR_dq(31 downto 0),
        DDR_dqs_n(3 downto 0) => DDR_dqs_n(3 downto 0),
        DDR_dqs_p(3 downto 0) => DDR_dqs_p(3 downto 0),
        DDR_odt => DDR_odt,
        DDR_ras_n => DDR_ras_n,
        DDR_reset_n => DDR_reset_n,
        DDR_we_n => DDR_we_n,
        FIXED_IO_ddr_vrn => FIXED_IO_ddr_vrn,
        FIXED_IO_ddr_vrp => FIXED_IO_ddr_vrp,
        FIXED_IO_mio(53 downto 0) => FIXED_IO_mio(53 downto 0),
        FIXED_IO_ps_clk => FIXED_IO_ps_clk,
        FIXED_IO_ps_porb => FIXED_IO_ps_porb,
        FIXED_IO_ps_srstb => FIXED_IO_ps_srstb,
        bclk => ac_bclk,
        iic_rtl_scl_i => iic_rtl_scl_i,
        iic_rtl_scl_o => iic_rtl_scl_o,
        iic_rtl_scl_t => iic_rtl_scl_t,
        iic_rtl_sda_i => iic_rtl_sda_i,
        iic_rtl_sda_o => iic_rtl_sda_o,
        iic_rtl_sda_t => iic_rtl_sda_t,
        lrck => ac_pblrc,
        mclk => ac_mclk,
        sdata => ac_pbdat
    );

    -- Tri-state buffer
    iic_rtl_scl_iobuf: component IOBUF
    port map (
        I => iic_rtl_scl_o,
        IO => ac_scl,
        O => iic_rtl_scl_i,
        T => iic_rtl_scl_t
    );
    iic_rtl_sda_iobuf: component IOBUF
    port map (
        I => iic_rtl_sda_o,
        IO => ac_sda,
        O => iic_rtl_sda_i,
        T => iic_rtl_sda_t
    );

end Behavioral;
