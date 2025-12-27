-- library ieee;
-- use ieee.std_logic_1164.all;

ENTITY demo_device is
  -- generic (
  --   SIZE : positive;
  --   RESET_ACTIVE_LEVEL : std_ulogic := '1'
  --   );
  port (
    --# {{clocks|}}
    Clock : in std_ulogic;
    Reset : in std_ulogic;

    --# {{control|Named section}}
    Enable : in std_ulogic;
    Data_in : in std_ulogic_vector(SIZE-1 downto 0);
    Data_out : out std_ulogic_vector(SIZE-1 downto 0)
    );
end entity;



architecture aasas_asaSA_A_ds_behavior of top_11 is

begin  -- architecture behavior 
  

end architecture aasas_asaSA_A_ds_behavior;
