#ifndef ALB_OBJ_H_
#define ALB_OBJ_H_

///////////////////////////////////////////////////////////////////////////////
//  author: Aidan Boneham
//  desc:   
//  date:   2/15/2015 11:55:03 AM
//  notice: (C) Aidan Boneham 2014-2015
///////////////////////////////////////////////////////////////////////////////

const int PATH_SIZE = 256;

typedef struct
{
    char name[PATH_SIZE];

    float ambient[3];
    float diffuse[3];
    float specular[3];
    float transmittance[3];
    float emission[3];
    float shininess;
    float ior;                // index of refraction
    float dissolve;           // 1 == opaque; 0 == fully transparent

    // illumination model (see http://www.fileformat.info/format/material/)
    int illum;

    char ambient_texname[PATH_SIZE];
    char diffuse_texname[PATH_SIZE];
    char specular_texname[PATH_SIZE];
    char normal_texname[PATH_SIZE];

} material_t;

typedef struct
{
    float position[3];
    float texcoord[2];
    float normal[3];
} vertex_t;

enum offsets
{
    POSITION_OFFSET = 0,
    TEXCOORD_OFFSET = 12,
    NORMAL_OFFSET = 20,

    OFFSET_TOTAL = sizeof(vertex_t)
};

typedef struct
{
    unsigned int* indices;
    unsigned int index_count;

    vertex_t* vertex_data;
    unsigned int vertex_count;

    int material_id;

} mesh_t;

typedef struct
{
    char name[PATH_SIZE];
    mesh_t mesh;
} shape_t;

typedef struct
{
    material_t* materials;
    unsigned int material_count;

    shape_t* shapes;
    unsigned int shape_count;

} obj_file_t;

obj_file_t alb_obj_load_file(char* filename);
void alb_obj_free_file(obj_file_t* file);
void alb_obj_generate_normals(mesh_t* mesh);

#endif

#ifdef ALB_OBJ_IMPLEMENTATION

#define Assert(B) if (!(B)){*(char*)0 = 0;}
/*
void alb_obj_generate_normals(mesh_t* mesh)
{
    for (unsigned int vert_index = 0 ;
            vert_index < mesh->vertex_count ;
            ++vert_index )
    {
        mesh->vertex_data[vert_index].normal[0] = 0;
        mesh->vertex_data[vert_index].normal[1] = 0;
        mesh->vertex_data[vert_index].normal[2] = 0;
    }

	for (unsigned int index = 0;
            index < mesh->index_count ; )
    {
        unsigned int index0 = mesh->indices[index + 0];
        unsigned int index1 = mesh->indices[index + 1];
        unsigned int index2 = mesh->indices[index + 2];

        vertex v0 = mesh->vertex_data[index0];
        vertex v1 = mesh->vertex_data[index1];
        vertex v2 = mesh->vertex_data[index2];
		
        float v0t1[3] = *((Vec3*)v1.position) - *((Vec3*)v0.position);
        float v0t2[3] = *((Vec3*)v2.position) - *((Vec3*)v0.position);

        Vec3 normal = Cross(v0t1, v0t2);

        v0.normal[0] += normal.x;
        v0.normal[1] += normal.y;
        v0.normal[2] += normal.z;

        v1.normal[0] += normal.x;
        v1.normal[1] += normal.y;
        v1.normal[2] += normal.z;

        v2.normal[0] += normal.x;
        v2.normal[1] += normal.y;
        v2.normal[2] += normal.z;

        mesh->vertex_data[index0] = v0;
        mesh->vertex_data[index1] = v1;
        mesh->vertex_data[index2] = v2;

        index += 3;
    }

    for (unsigned int vert_index = 0 ;
            vert_index < mesh->vertex_count ;
            ++vert_index )
    {
        Normalize((Vec3*)mesh->vertex_data[vert_index].normal);
    }
}*/


struct FileData
{
    void *data;
    size_t size;
};

FileData
ReadEntireTextFile(const char * file_path)
{
    FileData result = {};
    FILE * file = fopen(file_path, "r");
    Assert(file); //The file didn't exist!

    fseek(file, 0, SEEK_END);
    result.size = ftell(file);
    result.data = calloc(1, result.size);
    fseek(file, 0, SEEK_SET);
    result.size = fread(result.data, 1, result.size, file);
    fclose(file);
	return result;
}

void
CloseFile(FileData* file_data)
{
    free(file_data->data);
    *file_data = {};
}

struct LineInfo
{
    char* line;
    char* next_line;
    size_t line_length;
};

struct Index
{
    union
    {
        struct
        {
            int vertex_index;
            int texcoord_index;
            int normal_index;
        };

        int indices[3];
    };
};

LineInfo
GetNextLineInfo(char* curr_location, char* end)
{
    LineInfo result = {};

    result.line = curr_location;
    result.next_line = result.line;

    while( *result.next_line++ != '\n' && result.next_line < end )
    {
        ++result.line_length;
    }

    while( ( *result.next_line == '\t' || *result.next_line == ' ') 
            && result.next_line < end )
    {
        ++result.next_line;
    }

    return result;
}

inline
int ReadInt(char* str, size_t len)
{
    int sign = 1;
    if ( str[0] == '-' )
    {
        sign = -1;
        --len;
        ++str;
    }

    int result = 0;

    switch (len) 
    { 
        case 10:    result += (str[len-10] - '0') * 1000000000;
        case  9:    result += (str[len- 9] - '0') * 100000000;
        case  8:    result += (str[len- 8] - '0') * 10000000;
        case  7:    result += (str[len- 7] - '0') * 1000000;
        case  6:    result += (str[len- 6] - '0') * 100000;
        case  5:    result += (str[len- 5] - '0') * 10000;
        case  4:    result += (str[len- 4] - '0') * 1000;
        case  3:    result += (str[len- 3] - '0') * 100;
        case  2:    result += (str[len- 2] - '0') * 10;
        case  1:    result += (str[len- 1] - '0');
                    result *= sign;
        case 0:
                    break;

        default:
                    Assert(0);
    }

    return result;
}

Index ReadIndex(char * str)
{
    Index result = {};
    char* marker = str;

    while ( *marker++ != '/' );
    result.vertex_index = ReadInt(str, marker - str - 1);

    str = marker;
    while ( *marker++ != '/' && *(marker - 1) != ' ' && *(marker - 1) != '\n');
    result.texcoord_index = ReadInt(str, marker - str - 1);

    if ( *(marker-1) != ' ' && *(marker-1) != '\n' )
    {
        str = marker;
        while ( *marker++ != '\n' && *(marker-1) != ' ' );
        result.normal_index = ReadInt(str, marker - str - 1);
    } 

    return result;
}

void
ReadWord(char* dst, char* src, unsigned int max)
{
    unsigned int copied = 0;
    while(*src != ' ' && *src != '\t' && *src != '\n' && copied < max)
    {
        *dst++ = *src++;
        copied++;
    }

    *dst = '\0';
}

float ReadFloat(char* str)
{
    char digits[32];
    int curr_digit = 0;
    int decimal = -1;
    int negative = 1;
    int exponent = 0;

    while( *str != ' ' && *str != '\t' && *str != '\n' )
    {
        if ( *str == '-' )
        {
            Assert(curr_digit == 0);
            negative = -1;
        }
        else if ( *str == '+' )
        {
            Assert(curr_digit == 0);
            negative = 1;
        }
        else if ( *str >= '0' && *str <= '9' )
        {
            digits[curr_digit++] = *str - '0';
        }
        else if ( *str == '.' )
        {
            decimal = curr_digit;
        }
        else if ( *str == 'e' )
        {
            exponent = ReadInt(str + 1, 3);
            break;
        }
        else
        {
            Assert(0);
        }

        ++str;
    }

    float result = 0;
    float pow = decimal == -1 ? 1 : powf(10.f, (float)-(curr_digit - decimal));
    for ( int digit = curr_digit - 1;
            digit >= 0 ;
            --digit )
    {
        result += digits[digit] * pow;
        pow *= 10;
    }

    result *= negative;
    result *= powf(10.0f, (float)exponent);

    return result;
}

int IsNewWord(char* str)
{
    int result = ((*str != ' ' && *str != '\t') && (*(str - 1) == ' ' || *(str - 1) == '\t') );
    return result;
}

struct ShapeMarker
{
    unsigned int first_index;
    unsigned int last_index;
    int material_index;
    char name[PATH_SIZE];
};

struct VertexWithIndex
{
    vertex_t vert;
    unsigned int index;
};

int CompareVertexWithIndex(const void* first, const void* second)
{
    int result = memcmp(first, second, sizeof(VertexWithIndex) - sizeof(unsigned int));
    return result;
}

const int NAME_LENGTH = 256;
const int MAX_MATERIALS = 256;

int GetMaterialIndex(char* name, char* material_names, unsigned int material_name_count)
{
    int result = -1;

    for (unsigned int name_index = 0 ; 
            name_index < material_name_count ;
            ++name_index)
    {
        if ( strcmp(name, material_names + NAME_LENGTH * name_index) == 0 )
        {
            result = name_index;
            break;
        }
    }

    return result;
}

unsigned int
GetLastSlash(const char* file)
{
    size_t path_len = strlen(file);

    unsigned int result = 0;

    for ( size_t char_index = path_len - 1;
            char_index >= 0;
            --char_index)
    {
        if ( file[char_index] == '/' || file[char_index] == '\\' )
        {
            result = (unsigned int)char_index + 1;
            break;
        }
    }

    return result;
}

inline int
IsEmpty(LineInfo line)
{
    int result = ( line.line_length == 0 || line.line[0] == '#' );
    return result;
}

int
MoveToNextLine(LineInfo* line, FileData text_file)
{
    char* end = (char*)text_file.data + text_file.size;

    int result = true;

    do
    {
        if(line->next_line <= end)
        {
            line->line = line->next_line;
            *line = GetNextLineInfo((char*)line->line, end);
        }
        else
        {
            result = false;
            break;
        }

    } while ( IsEmpty(*line) );

    return result;
}

unsigned int 
LoadMaterials(FileData mat_file, char* material_names, material_t* materials, unsigned int max_materials)
{
    LineInfo line = {};
    line.line = (char*)mat_file.data;
    line.next_line = line.line;
    char* end = (char*)mat_file.data + mat_file.size;

    unsigned int loaded_materials = 0;
    material_t* curr_material = materials - 1;

    while(MoveToNextLine(&line, mat_file))
    {
        if ( strncmp(line.line, "newmtl", 6) == 0 )
        {
            if (loaded_materials >= max_materials )
            {
                break;
            }

            loaded_materials++;
            curr_material++;            

            for ( unsigned int i = 6 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    char* curr_name = material_names + (NAME_LENGTH * (loaded_materials - 1));
                    ReadWord(curr_material->name, &line.line[i], NAME_LENGTH);
                    ReadWord(curr_name, &line.line[i], NAME_LENGTH);
                    break;
                }
            }
 
        }
        else if ( strncmp(line.line, "Ns", 2 ) == 0 )
        {
            for ( unsigned int i = 2 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    curr_material->shininess = ReadFloat(&line.line[i]);
                    break;
                }
            }
        }
        else if ( strncmp(line.line, "Ni", 2 ) == 0 )
        {
            for ( unsigned int i = 2 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    curr_material->ior = ReadFloat(&line.line[i]);
                    break;
                }
            }
        }
        else if ( line.line[0] == 'd' )
        {
            for ( unsigned int i = 1 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    curr_material->dissolve = ReadFloat(&line.line[i]);
                    break;
                }
            }
        }
        else if ( strncmp(line.line, "Tr", 2 ) == 0 )
        {
            for ( unsigned int i = 2 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    curr_material->dissolve = ReadFloat(&line.line[i]);
                    break;
                }
            }
        }
        else if ( strncmp(line.line, "illum", 5 ) == 0 )
        {
            for ( unsigned int i = 2 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    curr_material->illum = (int)ReadFloat(&line.line[i]);
                    break;
                }
            }
        }
        else if ( strncmp(line.line, "Ka", 2 ) == 0 )
        {
            float* ambient = curr_material->ambient;

            for ( unsigned int i = 2 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    *ambient++ = ReadFloat(&line.line[i]);
                }
            }
        }
        else if ( strncmp(line.line, "Kd", 2 ) == 0 )
        {
            float* diffuse = curr_material->diffuse;

            for ( unsigned int i = 2 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    *diffuse++ = ReadFloat(&line.line[i]);
                }
            }
        }
        else if ( strncmp(line.line, "Ks", 2 ) == 0 )
        {
            float* specular = curr_material->specular;

            for ( unsigned int i = 2 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    *specular++ = ReadFloat(&line.line[i]);
                }
            }
        }
        else if ( strncmp(line.line, "Ke", 2 ) == 0 )
        {
            float* emissive = curr_material->emission;

            for ( unsigned int i = 2 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    *emissive++ = ReadFloat(&line.line[i]);
                }
            }
        }
        else if ( strncmp(line.line, "Kr", 2 ) == 0 )
        {
            float* transmittance = curr_material->transmittance;

            for ( unsigned int i = 2 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    *transmittance++ = ReadFloat(&line.line[i]);
                }
            }
        }
        else if ( strncmp(line.line, "map_Ka", 6) == 0 )
        {
            for ( unsigned int i = 2 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    ReadWord(curr_material->ambient_texname, &line.line[i], PATH_SIZE);
                    break;
                }
            }
        }
        else if ( strncmp(line.line, "map_Kd", 6) == 0 )
        {
            for ( unsigned int i = 2 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    ReadWord(curr_material->diffuse_texname, &line.line[i], PATH_SIZE);
                    break;
                }
            }
        }
        else if ( strncmp(line.line, "map_Ks", 6) == 0 )
        {
            for ( unsigned int i = 2 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    ReadWord(curr_material->specular_texname, &line.line[i], PATH_SIZE);
                    break;
                }
            }
        }
        else if ( strncmp(line.line, "map_Ns", 6) == 0 )
        {
            for ( unsigned int i = 2 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    ReadWord(curr_material->normal_texname, &line.line[i], PATH_SIZE);
                    break;
                }
            }
        }

    }

    return loaded_materials;
}

obj_file_t
alb_obj_load_file(const char* filename)
{
    //open the file
    FileData obj_file = ReadEntireTextFile(filename);

    unsigned int last_slash = GetLastSlash(filename);
    char file_directory[PATH_SIZE] = {};

    strncpy(file_directory, filename, last_slash);

    //count vertex/index data requirements
    char* end = (char*)obj_file.data + obj_file.size;
    size_t binary_block_bytes = 0;
    size_t group_size = 0;
    size_t vertex_size = 0;
    size_t normal_size = 0;
    size_t texcoord_size = 0;
    size_t index_size = 0;
    size_t material_size = 0;

    material_t materials[MAX_MATERIALS] = {};
    unsigned int material_count = 0;
    char material_names[MAX_MATERIALS][NAME_LENGTH];

    LineInfo line = {};
    line.line = (char*)obj_file.data;
    line.next_line = (char*)obj_file.data;

    int curr_group_has_material = false;

    while(MoveToNextLine(&line, obj_file))
    {
        if ( line.line[0] == 'v' )
        {
            if (line.line[1] == ' ' || line.line[1] == '\t')
            {
                //vertex
                binary_block_bytes += sizeof(float) * 3;
                vertex_size += sizeof(float) * 3;
            }
            else if ( line.line[1] == 'n' )
            {
                //normal
                binary_block_bytes += sizeof(float) * 3;
                normal_size += sizeof(float) * 3;
            }
            else if ( line.line[1] == 't' )
            {
                //texcoord
                binary_block_bytes += sizeof(float) * 2;
                texcoord_size += sizeof(float) * 2;
            }
        }
        else if (line.line[0] == 'f')
        {
            unsigned int index_count = 0;

            for ( unsigned int i = 1 ; i < line.line_length ; ++i )
            {
                if (IsNewWord(&line.line[i]))
                {
                    ++index_count;
                }
            }

            if (index_count == 3 )
            {
                binary_block_bytes += 3 * sizeof(Index);
                index_size += 3 * sizeof(Index);
            }
            else if (index_count == 4 )
            {
                binary_block_bytes += 6 * sizeof(Index);
                index_size += 6 * sizeof(Index);
            }
            else
            {
                Assert(0);
            }
        }
        else if ( line.line[0] == 'g' )
        {
            binary_block_bytes += sizeof(ShapeMarker);
            group_size += sizeof(ShapeMarker);
            curr_group_has_material = false;
        }
        else if (strncmp(line.line, "usemtl", 6) == 0)
        {
            if (curr_group_has_material)
            {
                binary_block_bytes += sizeof(ShapeMarker);
                group_size += sizeof(ShapeMarker);
            }
            curr_group_has_material = true;
        }
        else if (strncmp(line.line, "mtllib", 6) == 0)
        {
            for (unsigned int i = 6 ; i < line.line_length ; ++i )
            {
                char *matlib_filename = &file_directory[last_slash];

                if ( IsNewWord(&line.line[i]) )
                {
                    ReadWord(matlib_filename, &line.line[i], PATH_SIZE);

                    //read in material file
                    FileData material_file = ReadEntireTextFile(file_directory);

                    //pull in material data
                    material_count += LoadMaterials(material_file, 
                            material_names[material_count],
                            &materials[material_count], 
                            MAX_MATERIALS - material_count);


                    //close file
                    CloseFile(&material_file);
                    break;
                }
            }
        }
    }

    if ( group_size == 0 )
    {
        binary_block_bytes += sizeof(ShapeMarker);
        group_size += sizeof(ShapeMarker);
    }

    //allocate memory block
    char * binary_block = (char*)malloc(binary_block_bytes); 

    ShapeMarker* groups = (ShapeMarker*)(binary_block);
    float* vertices = (float*)((char*)groups + group_size);
    float* normals = (float*)((char*)vertices + vertex_size);
    float* texcoords = (float*)((char*)normals + normal_size);
    Index* indices = (Index*)((char*)texcoords + texcoord_size);

    ShapeMarker* curr_group = groups;
    float* curr_vertex = vertices;
    float* curr_normal = normals;
    float* curr_texcoord = texcoords;
    Index* curr_index = indices;

    //convert text into binary
    line = {};
    line.line = (char*)obj_file.data;
    line.next_line = (char*)obj_file.data;

	*curr_group = {};

    curr_group_has_material = false;

    while(MoveToNextLine(&line, obj_file))
    {
        if ( line.line[0] == 'v' )
        {
            if (line.line[1] == ' ' || line.line[1] == '\t')
            {
                //vertex
                int pulled_verts = 0;
                for ( unsigned int i = 1 ; i < line.line_length ; ++i )
                {
                    if ( IsNewWord(&line.line[i]) )
                    {
                        ++pulled_verts;
                        *curr_vertex++ = ReadFloat(&line.line[i]);
                        if ( pulled_verts == 3 )
                        {
                            break;
                        }
                    }
                }
            }
            else if ( line.line[1] == 'n' )
            {
                //normal
                int pulled_normals = 0;
                for ( unsigned int i = 1 ; i < line.line_length ; ++i )
                {
                    if ( IsNewWord(&line.line[i]) )
                    {
                        ++pulled_normals;
                        *curr_normal++ = ReadFloat(&line.line[i]);
                        if ( pulled_normals == 3 )
                        {
                            break;
                        }
                    }
                }
            }
            else if ( line.line[1] == 't' )
            {
                //texcoord
                int pulled_texcoords = 0;
                for ( unsigned int i = 1 ; i < line.line_length ; ++i )
                {
                    if ( IsNewWord(&line.line[i]) )
                    {
                        ++pulled_texcoords;
                        *curr_texcoord++ = ReadFloat(&line.line[i]);
                        if (pulled_texcoords == 2 )
                        {
                            break;
                        }
                    }
                }
            }
        }
        else if (line.line[0] == 'f')
        {
            Index temp_index_list[4] = {};
            int temp_curr_index = 0;

            for ( unsigned int i = 1 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    temp_index_list[temp_curr_index] = ReadIndex(&line.line[i]);

                    int latest_index[3] = 
                    {
                        (int)(curr_vertex - vertices) / 3,
                        (int)(curr_texcoord - texcoords) / 2,
                        (int)(curr_normal - normals) / 3
                    };

                    for ( unsigned int i = 0 ; i < 3 ; ++i )
                    {
                        if ( temp_index_list[temp_curr_index].indices[i] < 0 )
                        {
                            temp_index_list[temp_curr_index].indices[i] =
                                (latest_index[i] + temp_index_list[temp_curr_index].indices[i]) + 1;
                        }
                    }

                    temp_curr_index++;
                }
            }

            if ( temp_curr_index == 4 )
            {
                *curr_index++ = temp_index_list[0];
                *curr_index++ = temp_index_list[1];
                *curr_index++ = temp_index_list[2];
                *curr_index++ = temp_index_list[0];
                *curr_index++ = temp_index_list[2];
                *curr_index++ = temp_index_list[3];
            }
            else if (temp_curr_index == 3 )
            {
                *curr_index++ = temp_index_list[0];
                *curr_index++ = temp_index_list[1];
                *curr_index++ = temp_index_list[2];
            }
            else
            {
                Assert(0);
            }
        }
        else if (line.line[0] == 'g' )
        {
            unsigned int index = unsigned int(curr_index - indices);

            if ( index == 0 && curr_group == groups )
            {
            }
            else
            {
                curr_group->last_index = index;
                ++curr_group;
            }

            curr_group->first_index = index;

            for ( unsigned int i = 1 ; i < line.line_length ; ++i )
            {
                if ( IsNewWord(&line.line[i]) )
                {
                    ReadWord(curr_group->name, &line.line[i], PATH_SIZE);
                    break;
                }
            }

            curr_group_has_material = false;
        }
        else if (strncmp(line.line, "usemtl", 6) == 0)
        {
            char material_name[PATH_SIZE] = {};

            for (unsigned int i = 6 ; i < line.line_length ; ++i )
            {
                if (IsNewWord(&line.line[i]))
                {
                    ReadWord(material_name, &line.line[i], PATH_SIZE);
                }
            }

            if (!curr_group_has_material)
            {
                curr_group->material_index = GetMaterialIndex(material_name, (char*)material_names, material_count);
            }
            else
            {
                unsigned int index = unsigned int(curr_index - indices);
                curr_group->last_index = index;
                ++curr_group;
                curr_group->first_index = index;
                curr_group->material_index = GetMaterialIndex(material_name, (char*)material_names, material_count);
            }

            curr_group_has_material = true;
        }
    }

    CloseFile(&obj_file);

    //fix indices and generate final data
    curr_group->last_index = (unsigned int)(curr_index - indices);

    unsigned int total_index_count = (unsigned int)(curr_index - indices);
    size_t expanded_data_bytes = total_index_count * sizeof(VertexWithIndex);

    VertexWithIndex* expanded_data = (VertexWithIndex*)malloc(expanded_data_bytes);

    unsigned int group_count = (unsigned int)group_size / sizeof(ShapeMarker);

    for ( unsigned int shape_index = 0 ;
            shape_index < group_count ;
            ++shape_index )
    {
        for (unsigned int index = groups[shape_index].first_index ; 
                index < groups[shape_index].last_index ; 
                ++index )
        {
            Index ind = indices[index];
            
            expanded_data[index].vert = {};

            if ( ind.vertex_index != 0 )
            {
                expanded_data[index].vert.position[0] = vertices[(ind.vertex_index - 1) * 3 + 0];
                expanded_data[index].vert.position[1] = vertices[(ind.vertex_index - 1) * 3 + 1];
                expanded_data[index].vert.position[2] = vertices[(ind.vertex_index - 1) * 3 + 2];
            }

            if ( ind.texcoord_index != 0)
            {
                expanded_data[index].vert.texcoord[0] = texcoords[(ind.texcoord_index - 1) * 2 + 0];
                expanded_data[index].vert.texcoord[1] = texcoords[(ind.texcoord_index - 1) * 2 + 1];
            }

            if ( ind.normal_index != 0 )
            {
                expanded_data[index].vert.normal[0] = normals[(ind.normal_index - 1) * 3 + 0];
                expanded_data[index].vert.normal[1] = normals[(ind.normal_index - 1) * 3 + 1];
                expanded_data[index].vert.normal[2] = normals[(ind.normal_index - 1) * 3 + 2];
            }

            expanded_data[index].index = index - groups[shape_index].first_index;
            Assert(expanded_data[index].index < total_index_count);
        }
    }

    VertexWithIndex* base = expanded_data;

    obj_file_t result = {};
    result.shape_count = group_count;
    
    size_t index_data_bytes = 0;

    for ( unsigned int shape_index = 0 ;
            shape_index < group_count ;
            ++shape_index )
    {
        unsigned int group_index_count = 
            (groups[shape_index].last_index - groups[shape_index].first_index);

        index_data_bytes += sizeof(unsigned int) * group_index_count;
        qsort(base, group_index_count, sizeof(VertexWithIndex), CompareVertexWithIndex);
        base += group_index_count;
    }

    unsigned int* index_data = (unsigned int*)malloc(index_data_bytes);
    unsigned int* curr_index_base = index_data;

    size_t shapes_bytes = group_count * sizeof(shape_t);
    result.shapes = (shape_t*)malloc(shapes_bytes);

    size_t material_bytes = material_count * sizeof(material_t);
    result.material_count = material_count;
    result.materials = (material_t*)malloc(material_bytes);
    memcpy(result.materials, materials, material_bytes);

    VertexWithIndex* write_cursor = expanded_data;
    for ( unsigned int shape_index = 0 ;
            shape_index < group_count ;
            ++shape_index )
    {
        VertexWithIndex* write_cursor = &expanded_data[groups[shape_index].first_index];
        unsigned int curr_index = 0;
        curr_index_base[write_cursor->index] = curr_index;

        unsigned int group_index_count = 
            (groups[shape_index].last_index - groups[shape_index].first_index);

        result.shapes[shape_index].mesh.index_count = group_index_count;
        result.shapes[shape_index].mesh.vertex_count = group_index_count;
        result.shapes[shape_index].mesh.indices = curr_index_base;

        for (unsigned int index = groups[shape_index].first_index + 1;
                index < groups[shape_index].last_index ; 
                ++index )
        {
            if ( memcmp(write_cursor, &expanded_data[index], sizeof(vertex_t)) == 0 )
            {
                curr_index_base[expanded_data[index].index] = curr_index;
                result.shapes[shape_index].mesh.vertex_count--;
            }
            else
            {
                //IMPORTANT(aidan):this MUST be updated if the VertexWithIndex structure is ever changed
				Assert(sizeof(VertexWithIndex) == (sizeof(vertex_t) + sizeof(unsigned int)));

				write_cursor = (VertexWithIndex*)((char*)write_cursor + sizeof(vertex_t));
                *write_cursor = expanded_data[index];
                ++curr_index;
                curr_index_base[expanded_data[index].index] = curr_index;
                write_cursor->index = index;
            }
        }
        curr_index_base += group_index_count;
    }

    size_t vertex_data_size = 0;

    for ( unsigned int shape_index = 0 ;
            shape_index < group_count ;
            ++shape_index )
    {
		vertex_data_size += result.shapes[shape_index].mesh.vertex_count * sizeof(vertex_t);
    }

	vertex_t* vertex_data = (vertex_t*)malloc(vertex_data_size);
	vertex_t* next_base_vertex = (vertex_t*)vertex_data;

    for ( unsigned int shape_index = 0 ;
            shape_index < group_count ;
            ++shape_index )
    {
        vertex_t* src = (vertex_t*)&expanded_data[groups[shape_index].first_index];
        strcpy(result.shapes[shape_index].name, groups[shape_index].name);

        result.shapes[shape_index].mesh.material_id = groups[shape_index].material_index;

        result.shapes[shape_index].mesh.vertex_data = next_base_vertex;
		memcpy(next_base_vertex, src, sizeof(vertex_t) * result.shapes[shape_index].mesh.vertex_count);

        next_base_vertex += result.shapes[shape_index].mesh.vertex_count;
    }
    
    free(expanded_data);
    free(binary_block);

    return result;
}

void 
alb_obj_free_file(obj_file_t* file)
{
	Assert(file);
	if (file->shape_count)
	{
		free(file->shapes[0].mesh.vertex_data);
		free(file->shapes[0].mesh.indices);
		free(file->materials);
		free(file->shapes);
	}
    *file = {};
}

#endif

