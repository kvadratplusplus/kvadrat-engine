#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
    float x, y, z;
} V;

typedef struct
{
    float x, y;
} VT;

typedef struct
{
    float x, y, z;
} VN;

static FILE * input = NULL;
static FILE * output = NULL;
V * vbuf = NULL;
VT * vtbuf = NULL;
VN * vnbuf = NULL;

void panic(void)
{
    if (input)
        fclose(input);

    if (output)
        fclose(output);

    if (vbuf)
        free(vbuf);

    if (vnbuf)
        free(vnbuf);

    if (vtbuf)
        free(vtbuf);
}

int main(int argc, char * argv[])
{
    if(argc != 4) {
        printf(
            "Usage: obj2kmdl input_file output_file vertices\n"
            "input_file must be in .obj format\n"
            "output_file is in .kmdl format\n"
            "vertices = vertices_max from your \"config.cfg\"\n"
        );
        return 1;
    }
    atexit(panic);
    
    //.obj
    input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Error: File \"%s\" does not exist\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    //.kmdl
    output = fopen(argv[2], "wb");
    if (!output) {
        fprintf(stderr, "Error: File \"%s\" does not exist\n", argv[2]);
        fclose(input);
        exit(EXIT_FAILURE);
    }

    size_t vertices_max;
    if (sscanf(argv[3], "%zu", &vertices_max) == 0) {
        fprintf(stderr, "Error: invalid vertices_max: \"%s\"\n", argv[3]);
        exit(EXIT_FAILURE);
    }

    char line[260];
    vbuf = (V*)calloc(vertices_max, sizeof(V));
    vtbuf = (VT*)calloc(vertices_max, sizeof(VT));
    vnbuf = (VN*)calloc(vertices_max, sizeof(VN));

    if (!vbuf || !vtbuf || !vnbuf) {
        fprintf(stderr, "Error: Could not allocate memory\n");
        fclose(input);
        fclose(output);
        exit(EXIT_FAILURE);
    }    

    //char temp[16];
    float x, y, z;
    size_t v_counter = 0;
    size_t vt_counter = 0;
    size_t vn_counter = 0;
    size_t f_counter = 0;

    while (fgets(line, 260, input)) {
        //memory leak checkers
        if (v_counter >= vertices_max) {
            fprintf(stderr, "Error: Model contains too many vertices (%zu / %zu)\n",
                v_counter, vertices_max);
            exit(EXIT_FAILURE);
        }
        if(vt_counter >= vertices_max) {
            fprintf(stderr, "Error: Model contains too many texture coords (%zu / %zu)\n",
                vt_counter, vertices_max);
            exit(EXIT_FAILURE);
        }
        if(vn_counter >= vertices_max) {
            fprintf(stderr, "Error: Model contains too many vertex normals (%zu / %zu)\n",
                vn_counter, vertices_max);
            exit(EXIT_FAILURE);
        }
        //parser
        if(line[0] == 'v' && line[1] == ' ') {
            sscanf(&line[1], "%f %f %f", &x, &y, &z);
            vbuf[v_counter].x = x;
            vbuf[v_counter].y = y;
            vbuf[v_counter].z = z;
            v_counter++;
        } else if(line[0] == 'v' && line[1] == 't') {
            sscanf(&line[2], "%f %f", &x, &y);
            vtbuf[vt_counter].x = x;
            vtbuf[vt_counter].y = y;
            vt_counter++;
        } else if(line[0] == 'v' && line[1] == 'n') {
            sscanf(&line[2], "%f %f %f", &x, &y, &z);
            vnbuf[vn_counter].x = x;
            vnbuf[vn_counter].y = y;
            vnbuf[vn_counter].z = z;
            vn_counter++;
        } else if(line[0] == 'f' && line[1] == ' ') {
            size_t v1, vt1, vn1;
            size_t v2, vt2, vn2;
            size_t v3, vt3, vn3;
            sscanf(
                &line[1],
                "%zu/%zu/%zu %zu/%zu/%zu %zu/%zu/%zu",
                &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3
            );
            
            //write to file
            //##### first face #####
            //# positions #
            fwrite(&vbuf[v1 - 1].x, sizeof(float), 1, output);
            fwrite(&vbuf[v1 - 1].y, sizeof(float), 1, output);
            fwrite(&vbuf[v1 - 1].z, sizeof(float), 1, output);
            //# texture coords #
            fwrite(&vtbuf[vt1 - 1].x, sizeof(float), 1, output);
            fwrite(&vtbuf[vt1 - 1].y, sizeof(float), 1, output);
            //# normal vectors #
            fwrite(&vnbuf[vn1 - 1].x, sizeof(float), 1, output);
            fwrite(&vnbuf[vn1 - 1].y, sizeof(float), 1, output);
            fwrite(&vnbuf[vn1 - 1].z, sizeof(float), 1, output);

            //##### second face #####
            //# positions #
            fwrite(&vbuf[v2 - 1].x, sizeof(float), 1, output);
            fwrite(&vbuf[v2 - 1].y, sizeof(float), 1, output);
            fwrite(&vbuf[v2 - 1].z, sizeof(float), 1, output);
            //# texture coords #
            fwrite(&vtbuf[vt2 - 1].x, sizeof(float), 1, output);
            fwrite(&vtbuf[vt2 - 1].y, sizeof(float), 1, output);
            //# normal vectors #
            fwrite(&vnbuf[vn2 - 1].x, sizeof(float), 1, output);
            fwrite(&vnbuf[vn2 - 1].y, sizeof(float), 1, output);
            fwrite(&vnbuf[vn2 - 1].z, sizeof(float), 1, output);

            //##### third face #####
            //# positions #
            fwrite(&vbuf[v3 - 1].x, sizeof(float), 1, output);
            fwrite(&vbuf[v3 - 1].y, sizeof(float), 1, output);
            fwrite(&vbuf[v3 - 1].z, sizeof(float), 1, output);
            //# texture coords #
            fwrite(&vtbuf[vt3 - 1].x, sizeof(float), 1, output);
            fwrite(&vtbuf[vt3 - 1].y, sizeof(float), 1, output);
            //# normal vectors #
            fwrite(&vnbuf[vn3 - 1].x, sizeof(float), 1, output);
            fwrite(&vnbuf[vn3 - 1].y, sizeof(float), 1, output);
            fwrite(&vnbuf[vn3 - 1].z, sizeof(float), 1, output);

            f_counter++;
        } else {
            continue;
        }
        if(f_counter * 3 >= vertices_max) {
            //this means that the engine will not be able to load the model properly
            fprintf(stderr, "Warning: Model has too many vertices (%zu / %zu)\n",
                f_counter * 3, vertices_max);
            //exit(EXIT_FAILURE);
        }
    }

    //I think this is wrong
    printf(
        "\n-------------------------------\n"
        "Stats:\n"
        "-------------------------------\n"
        "v  = %-6zu / %-6zu | (%05.2f%%)\n"
        "vt = %-6zu / %-6zu | (%05.2f%%)\n"
        "vn = %-6zu / %-6zu | (%05.2f%%)\n"
        "-------------------------------\n"
        "Total faces =    %-6zu\n"
        "Total vertices = %-6zu / %-6zu | (%05.2f%%)\n"
        "-------------------------------\n\n",
        v_counter, vertices_max,
        (float)v_counter / vertices_max * 100.0f,
        vt_counter, vertices_max,
        (float)vt_counter / vertices_max * 100.0f,
        vn_counter, vertices_max,
        (float)vn_counter / vertices_max * 100.0f,
        f_counter,
        f_counter * 3, vertices_max,
        (float)(f_counter * 3) / vertices_max * 100.0f
    );

    return 0;
}
