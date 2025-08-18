-- library ieee;
-- use ieee.std_logic_1164.all;
-- use ieee.numeric_std.all;

ENTITY complex_device IS
  generic (
    --# {{configuration|Global parameters}}
    DATA_WIDTH       : positive := 16;
    ADDR_WIDTH       : natural  := 8;
    ENABLE_LOGIC     : boolean  := true;
    RESET_POLARITY   : std_ulogic := '0';
    TIMEOUT_CYCLES   : integer range 0 to 1024 := 128
  );
  port (
    -------------------------------------------------------------------
    --# {{control|Main control signals}}
    -------------------------------------------------------------------
    clk      : in  std_ulogic;
    rst      : in  std_ulogic;
    enable   : in  std_ulogic := '1';
    
    -------------------------------------------------------------------
    --# {{data|Input and output buses}}
    -------------------------------------------------------------------
    data_in  : in  std_ulogic_vector(DATA_WIDTH-1 downto 0);
    addr_in  : in  unsigned(ADDR_WIDTH-1 downto 0);
    valid_in : in  std_ulogic;
    ready_out: out std_ulogic;
    
    data_out : out std_ulogic_vector(DATA_WIDTH-1 downto 0);
    valid_out: out std_ulogic;
    ready_in : in  std_ulogic;
    
    -------------------------------------------------------------------
    --# {{status|Error and debug signals}}
    -------------------------------------------------------------------
    error_flag   : out std_ulogic;
    debug_vector : out std_ulogic_vector(31 downto 0);
    
    -------------------------------------------------------------------
    --# {{advanced|Record and array examples}}
    -------------------------------------------------------------------
    -- unconstrained array of std_logic_vectors
    test_array : inout std_ulogic_vector(DATA_WIDTH-1 downto 0) 
                          := (others => '0');
    
    -- example of a 2D array-like port
    multiword  : in std_ulogic_vector((2*DATA_WIDTH)-1 downto 0)
  );
END ENTITY complex_device;



architecture aasas_asaSA_A_ds_behavior of top_11 is

begin  -- architecture behavior 
  

end architecture aasas_asaSA_A_ds_behavior;
