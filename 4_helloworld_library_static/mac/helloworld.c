#include <json/json.h>
#include <stdio.h>

int
main ()
{
  char *string = "{\"name\" : \"joys of programming\"}";
  json_object *jobj = json_tokener_parse (string);
  enum json_type type = json_object_get_type (jobj);
  printf ("type: %d\n", type);
  switch (type)
    {
    case json_type_null:
      printf ("json_type_null\n");
      break;
    case json_type_boolean:
      printf ("json_type_boolean\n");
      break;
    case json_type_double:
      printf ("json_type_double\n");
      break;
    case json_type_int:
      printf ("json_type_intn");
      break;
    case json_type_object:
      printf ("json_type_object\n");
      break;
    case json_type_array:
      printf ("json_type_array\n");
      break;
    case json_type_string:
      printf ("json_type_string\n");
      break;
    }
}
