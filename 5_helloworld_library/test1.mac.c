#include <stdio.h>
#include <json.h>

void json_parse(json_object * jobj);

int main(int argc, char * argv[])
{
    unsigned char temp_buff[1024] =

        "{" \
        "  \"firstName\": \"John\"," \
        "  \"address\": {" \
        "    \"city\": \"New York\"," \
        "  }," \
        "}";

    json_object * jobj_parse = json_tokener_parse((char*)temp_buff);
    printf("-- BEGIN DUMP-\n%s\n-- END DUMP --\n", json_object_to_json_string_ext(jobj_parse, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));

    json_parse(jobj_parse);
    
    json_object_put(jobj_parse);
    return 0;
}

void json_parse(json_object * jobj) {
    enum json_type type;
    json_object_object_foreach(jobj, key, val) {
        type = json_object_get_type(val);
        switch (type) {
        case json_type_object:
        {
            printf("SRIDHAR 5 type: json_type_object, \n");
            json_object * tmp_object;
            json_object_object_get_ex(jobj, key, &tmp_object);
            json_parse(tmp_object);
            json_object_put(tmp_object);
            break;
        }
        case json_type_string:
        {
            printf("type: json_type_string, \n");
            json_object * tmp_string;
            json_object_object_get_ex(jobj, key, &tmp_string);
            printf("key: %s\n",key);
            printf("value: %s\n",json_object_get_string(tmp_string));
            json_object_put(tmp_string);
            break;
        }
        default:
        {
            //printf("SRIDHAR 3 type: default, \n");
        }
        }
    }
}
