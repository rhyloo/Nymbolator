#include "main.h"
#include "tools.h"

/*
 * max - Returns the maximum of two integers.
 *
 * Parameters:
 *   a - first integer
 *   b - second integer
 *
 * Returns:
 *   The larger of a and b.
 */
int max(int a, int b){
  return a > b ? a:b;
}

/*
 * GetMaxNameSize - Finds the length of the longest name in an array of StructPort.
 *
 * Parameters:
 *   port       - pointer to the first element of an array of StructPort
 *   port_count - number of elements in the array
 *
 * Returns:
 *   The length of the longest 'name' field among all ports.
 */
int GetMaxNameSize(StructPort *port, int port_count) {
  int final_text_size = 0;
  int actual_text_size = 0;
  
  for (int i = 0; i < port_count; i++) {
    actual_text_size = strlen(port[i].name);
    if (actual_text_size > final_text_size) {
      final_text_size = actual_text_size;
    }
  }
  
  return final_text_size;
}


/*
 * GetMaxNumberPorts - Counts input and output ports and returns the larger count.
 *
 * Parameters:
 *   port       - pointer to the first element of an array of StructPort
 *   port_count - number of elements in the array
 *
 * Returns:
 *   The maximum of the number of input ("in") ports and output ("out") ports.
 *
 * Description:
 *   Iterates through the array of ports, counting how many have direction "in"
 *   and how many have direction "out". Then uses the max() function to return
 *   the larger of the two counts.
 */
int GetMaxNumberPorts(StructPort *port, int port_count){
  int number_port_in = 0;
  int number_port_out = 0;

  for (int i = 0; i < port_count; i++) {
    if(strcmp(port[i].direction,"in") == 0){
      number_port_in++;
    }else if(strcmp(port[i].direction,"out") == 0){
      number_port_out++;
    }
  }
  return max(number_port_in,number_port_out);
}

/*
 * GetNameSize - Returns the length of a string.
 *
 * Parameters:
 *   name - pointer to a null-terminated string
 *
 * Returns:
 *   The number of characters in the string, not counting the terminating '\0'.
 *
 * Description:
 *   Iterates through the string one character at a time, incrementing a counter
 *   until the null terminator is reached, then returns the total count.
 */
int GetNameSize(char *name) {
  int text_size = 0;
  for (; *name != '\0'; name++) {
    text_size++;
  }
  return text_size;
}
