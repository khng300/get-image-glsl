// TODO: decide of a clang-format

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "common.h"
#include "openglcontext.h"
#include "lodepng.h"
#include "json.hpp"
using json = nlohmann::json;

// These codes reflect the ones used in 'get-image-glfw' (older version)
#define COMPILE_ERROR_EXIT_CODE (101)
#define LINK_ERROR_EXIT_CODE (102)
#define RENDER_ERROR_EXIT_CODE (103)

/*---------------------------------------------------------------------------*/
// Parameters, argument parsing
/*---------------------------------------------------------------------------*/

static void defaultParams(Params& params) {
    params.width = 256;
    params.height = 256;
    params.version = 0;
    params.fragFilename = "";
    params.vertFilename = "";
    params.output = "output.png";
    params.exitCompile = false;
    params.exitLinking = false;
}

/*---------------------------------------------------------------------------*/

static void usage(char *name) {
    std::cout << "Usage: " << name << " [options] <shader>.frag" << std::endl;
    std::cout << std::endl;

    const char *msg =
        "The program will look for a JSON whose name is derived from the\n"
        "shader as '<shader>.json'. This JSON file can contain uniforms\n"
        "initialisations. If no JSON file is found, the program uses default\n"
        "values for some uniforms.\n"
        ;

    std::cout << msg;
    std::cout << std::endl;

    std::cout << "Options are:" << std::endl;

    const char *options[] = {
        "--exit-compile", "exit after compilation",
        "--exit-linking", "exit after linking",
        "--output file.png", "set PNG output file name",
        "--resolution <width> <height>", "set resolution, in Pixels",
        "--vertex shader.vert", "use a specific vertex shader",
    };

    for (unsigned i = 0; i < (sizeof(options) / sizeof(*options)); i++) {
        printf("  %-34.34s %s\n", options[i], options[i+1]);
        i++;
    }
}

/*---------------------------------------------------------------------------*/

static void setParams(Params& params, int argc, char *argv[]) {
    defaultParams(params);

    for (int i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        if (arg.compare(0, 2, "--") == 0) {
            if        (arg == "--exit-compile") {
                params.exitCompile = true;
            } else if (arg == "--exit-linking") {
                params.exitLinking = true;
            } else if (arg == "--output") {
                if ((i + 1) >= argc) { usage(argv[0]); crash("Missing value for option %s", "--output"); }
                params.output = argv[++i];
            } else if (arg == "--resolution") {
                if ((i + 2) >= argc) { usage(argv[0]); crash("Missing value for option %s", "--resolution"); }
                params.width = atoi(argv[++i]);
                params.height = atoi(argv[++i]);
            } else if (arg == "--vertex") {
                if ((i + 1) >= argc) { usage(argv[0]); crash("Missing value for option %s", "--vertex"); }
                params.vertFilename = argv[++i];
            } else {
                usage(argv[0]);
                crash("Invalid option: %s", argv[i]);
            }
            continue;
        }
        if (params.fragFilename == "") {
            params.fragFilename = arg;
        } else {
            usage(argv[0]);
            crash("Unexpected extra argument: %s", arg.c_str());
        }
    }

    if (params.fragFilename == "") {
        usage(argv[0]);
        crash("Missing fragment shader argument");
    }
}

/*---------------------------------------------------------------------------*/
// Helpers
/*---------------------------------------------------------------------------*/

bool isFile(std::string filename) {
    std::ifstream ifs(filename.c_str());
    return ifs.good();
}

/*---------------------------------------------------------------------------*/

void readFile(std::string& contents, const std::string& filename) {
    std::ifstream ifs(filename.c_str());
    if(!ifs) {
        crash("File not found: %s", filename.c_str());
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    contents = ss.str();
}

/*---------------------------------------------------------------------------*/

int getVersion(const std::string& fragContents) {
    size_t pos = fragContents.find('\n');
    if (pos == std::string::npos) {
        crash("cannot find end-of-line in fragment shader");
    }
    std::string sub = fragContents.substr(0, pos);
    if (std::string::npos == sub.find("#version")) {
        crash("cannot find ``#version'' in first line of fragment shader");
    }

    // TODO: use sscanf of c++ equivalent
    if (std::string::npos != sub.find("110")) { return 110; }
    if (std::string::npos != sub.find("120")) { return 120; }
    if (std::string::npos != sub.find("130")) { return 130; }
    if (std::string::npos != sub.find("140")) { return 140; }
    if (std::string::npos != sub.find("150")) { return 150; }
    if (std::string::npos != sub.find("330")) { return 330; }
    if (std::string::npos != sub.find("400")) { return 400; }
    if (std::string::npos != sub.find("410")) { return 410; }
    if (std::string::npos != sub.find("420")) { return 420; }
    if (std::string::npos != sub.find("430")) { return 430; }
    if (std::string::npos != sub.find("440")) { return 440; }
    if (std::string::npos != sub.find("450")) { return 450; }
    // The following are OpenGL ES
    if (std::string::npos != sub.find("100")) { return 100; }
    if (std::string::npos != sub.find("300")) { return 300; }
    crash("Cannot find a supported GLSL version in first line of fragment shader: ``%.80s''", sub.c_str());
}

/*---------------------------------------------------------------------------*/

void generateVertexShader(std::string& out, const Params& params) {
    static const std::string vertGenericContents = std::string(
        "vec2 _GLF_vertexPosition;\n"
        "void main(void) {\n"
        "    gl_Position = vec4(_GLF_vertexPosition, 0.0, 1.0);\n"
        "}\n"
        );

    if (params.vertFilename != "") {
        readFile(out, params.vertFilename);
        return;
    }

    std::stringstream ss;
    ss << "#version " << params.version;
    if (params.version == 300) {
        // Version 300 must have the "es" suffix, and qualifies the
        // _GLF_vertexPosition as "in" rather than "attribute"
        ss << " es" << std::endl << "in ";
    } else {
        ss << std::endl << "attribute ";
    }
    ss << vertGenericContents;
    out = ss.str();

    //std::cerr << "Generated vertex shader:\n" << out << std::endl;
}

/*---------------------------------------------------------------------------*/
// JSON uniforms
/*---------------------------------------------------------------------------*/

static void setJSONDefaultEntries(json& j, const Params& params) {

    json defaults = {
        {"injectionSwitch", {
                {"func", "glUniform2f"},
                {"args", {0.0f, 1.0f}}
            }},
        {"time", {
                {"func", "glUniform1f"},
                {"args", {0.0f}}
            }},
        {"mouse", {
                {"func", "glUniform2f"},
                {"args", {0.0f, 0.0f}}
            }},
        {"resolution", {
                {"func", "glUniform2f"},
                {"args", {float(params.width), float(params.height)}}
            }}
    };

    for (json::iterator it = defaults.begin(); it != defaults.end(); ++it) {
        if (j.count(it.key()) == 0) {
            j[it.key()] = it.value();
        }
    }
}

/*---------------------------------------------------------------------------*/

template<typename T>
T *getArray(const json& j) {
    T *a = new T[j.size()];
    for (unsigned i = 0; i < j.size(); i++) {
        a[i] = j[i];
    }
    return a;
}

/*---------------------------------------------------------------------------*/

#define GLUNIFORM_ARRAYINIT(funcname, uniformloc, gltype, jsonarray)    \
    gltype *a = getArray<gltype>(jsonarray);                            \
    funcname(uniformloc, jsonarray.size(), a);                          \
    delete [] a

/*---------------------------------------------------------------------------*/

void setUniformsJSON(const GLuint& program, const Params& params) {
    GLint nbUniforms;
    GL_SAFECALL(glGetProgramiv, program, GL_ACTIVE_UNIFORMS, &nbUniforms);
    if (nbUniforms == 0) {
        // If there are no uniforms to set, return now
        return;
    }

    // Read JSON file
    std::string jsonFilename(params.fragFilename);
    jsonFilename.replace(jsonFilename.end()-4, jsonFilename.end(), "json");
    json j = json({});
    if (isFile(jsonFilename)) {
        std::string jsonContent;
        readFile(jsonContent, jsonFilename);
        j = json::parse(jsonContent);
    } else {
        std::cerr << "Warning: file '" << jsonFilename << "' not found, will rely on default uniform values only" << std::endl;
    }

    setJSONDefaultEntries(j, params);

    GLint uniformNameMaxLength = 0;
    GL_SAFECALL(glGetProgramiv, program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformNameMaxLength);
    GLchar *uniformName = new GLchar[uniformNameMaxLength];
    GLint uniformSize;
    GLenum uniformType;

    for (int i = 0; i < nbUniforms; i++) {
        GL_SAFECALL(glGetActiveUniform, program, i, uniformNameMaxLength, NULL, &uniformSize, &uniformType, uniformName);

        if (j.count(uniformName) == 0) {
            crash("missing JSON entry for uniform: %s", uniformName);
        }
        if (j.count(uniformName) > 1) {
            crash("more than one JSON entry for uniform: %s", uniformName);
        }
        json uniformInfo = j[uniformName];

        // Check presence of func and args entries
        if (uniformInfo.find("func") == uniformInfo.end()) {
            crash("malformed JSON: no 'func' entry for uniform: %s", uniformName);
        }
        if (uniformInfo.find("args") == uniformInfo.end()) {
            crash("malformed JSON: no 'args' entry for uniform: %s", uniformName);
        }

        // Get uniform location
        GLint uniformLocation = glGetUniformLocation(program, uniformName);
        GL_CHECKERR("glGetUniformLocation");
        if (uniformLocation == -1) {
            crash("Cannot find uniform named: %s", uniformName);
        }

        // Dispatch to matching init function
        std::string uniformFunc = uniformInfo["func"];
        json args = uniformInfo["args"];

        // TODO: check that args has the good number of fields and type

        if (uniformFunc == "glUniform1f") {
            glUniform1f(uniformLocation, args[0]);
        } else if (uniformFunc == "glUniform2f") {
            glUniform2f(uniformLocation, args[0], args[1]);
        } else if (uniformFunc == "glUniform3f") {
            glUniform3f(uniformLocation, args[0], args[1], args[2]);
        } else if (uniformFunc == "glUniform4f") {
            glUniform4f(uniformLocation, args[0], args[1], args[2], args[3]);
        }

        else if (uniformFunc == "glUniform1i") {
            glUniform1i(uniformLocation, args[0]);
        } else if (uniformFunc == "glUniform2i") {
            glUniform2i(uniformLocation, args[0], args[1]);
        } else if (uniformFunc == "glUniform3i") {
            glUniform3i(uniformLocation, args[0], args[1], args[2]);
        } else if (uniformFunc == "glUniform4i") {
            glUniform4i(uniformLocation, args[0], args[1], args[2], args[3]);
        }

        // Note: GLES does not provide glUniformXui primitives
#ifndef GL_VERSION_ES_CM_1_0

        else if (uniformFunc == "glUniform1ui") {
          glUniform1ui(uniformLocation, args[0]);
        } else if (uniformFunc == "glUniform2ui") {
          glUniform2ui(uniformLocation, args[0], args[1]);
        } else if (uniformFunc == "glUniform3ui") {
          glUniform3ui(uniformLocation, args[0], args[1], args[2]);
        } else if (uniformFunc == "glUniform4ui") {
          glUniform4ui(uniformLocation, args[0], args[1], args[2], args[3]);
        }

#endif // ifndef GL_VERSION_ES_CM_1_0

        else if (uniformFunc == "glUniform1fv") {
            GLUNIFORM_ARRAYINIT(glUniform1fv, uniformLocation, GLfloat, args);
        } else if (uniformFunc == "glUniform2fv") {
            GLUNIFORM_ARRAYINIT(glUniform2fv, uniformLocation, GLfloat, args);
        } else if (uniformFunc == "glUniform3fv") {
            GLUNIFORM_ARRAYINIT(glUniform3fv, uniformLocation, GLfloat, args);
        } else if (uniformFunc == "glUniform4fv") {
            GLUNIFORM_ARRAYINIT(glUniform4fv, uniformLocation, GLfloat, args);
        }

        else if (uniformFunc == "glUniform1iv") {
            GLUNIFORM_ARRAYINIT(glUniform1iv, uniformLocation, GLint, args);
        } else if (uniformFunc == "glUniform2iv") {
            GLUNIFORM_ARRAYINIT(glUniform2iv, uniformLocation, GLint, args);
        } else if (uniformFunc == "glUniform3iv") {
            GLUNIFORM_ARRAYINIT(glUniform3iv, uniformLocation, GLint, args);
        } else if (uniformFunc == "glUniform4iv") {
            GLUNIFORM_ARRAYINIT(glUniform4iv, uniformLocation, GLint, args);
        }

        else {
            crash("unknown/unsupported uniform init func: %s", uniformFunc.c_str());
        }
        GL_CHECKERR(uniformFunc.c_str());
    }

    delete [] uniformName;
}

/*---------------------------------------------------------------------------*/
// OpenGL
/*---------------------------------------------------------------------------*/

const char *opengl_error_string(GLenum err) {
    switch (err) {
    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
    default: return "UNKNOW_ERROR";
    }
}

/*---------------------------------------------------------------------------*/

void printShaderError(GLuint shader) {
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    // The length includes the NULL character
    std::vector<GLchar> errorLog((size_t) length, 0);
    glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
    if(length > 0) {
        std::string s(&errorLog[0]);
        std::cout << s << std::endl;
    }
}

/*---------------------------------------------------------------------------*/

void printProgramError(GLuint program) {
    GLint length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    // The length includes the NULL character
    std::vector<GLchar> errorLog((size_t) length, 0);
    glGetProgramInfoLog(program, length, &length, &errorLog[0]);
    if(length > 0) {
        std::string s(&errorLog[0]);
        std::cout << s << std::endl;
    }
}

/*---------------------------------------------------------------------------*/

void openglRender(const Params& params, const std::string& fragContents) {

    const char *temp;
    GLint status = 0;

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GL_CHECKERR("glCreateShader");
    temp = fragContents.c_str();
    GL_SAFECALL(glShaderSource, fragmentShader, 1, &temp, NULL);
    GL_SAFECALL(glCompileShader, fragmentShader);

    GL_SAFECALL(glGetShaderiv, fragmentShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        printShaderError(fragmentShader);
        errcode_crash(COMPILE_ERROR_EXIT_CODE, "Fragment shader compilation failed (%s)", params.fragFilename.c_str());
    }
    if (params.exitCompile) {
        exit(EXIT_SUCCESS);
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GL_CHECKERR("glCreateShader");
    std::string vertContents;
    generateVertexShader(vertContents, params);
    temp = vertContents.c_str();
    GL_SAFECALL(glShaderSource, vertexShader, 1, &temp, NULL);
    GL_SAFECALL(glCompileShader, vertexShader);
    GL_SAFECALL(glGetShaderiv, vertexShader, GL_COMPILE_STATUS, &status);
    if (!status) {
        printShaderError(vertexShader);
        crash("Vertex shader compilation failed");
    }

    GLuint program = glCreateProgram();
    GL_CHECKERR("glCreateProgram");
    if (program == 0) {
        crash("glCreateProgram()");
    }
    GL_SAFECALL(glAttachShader, program, vertexShader);
    GL_SAFECALL(glAttachShader, program, fragmentShader);
    GL_SAFECALL(glLinkProgram, program);
    GL_SAFECALL(glGetProgramiv, program, GL_LINK_STATUS, &status);
    if (!status) {
        printProgramError(program);
        crash("glLinkProgram()");
    }
    if (params.exitLinking) {
        exit(EXIT_SUCCESS);
    }

    GLint vertPosLocInt = glGetAttribLocation(program, "_GLF_vertexPosition");
    GL_CHECKERR("glGetAttribLocation");
    if (vertPosLocInt == -1) {
        crash("Cannot find position of _GLF_vertexPosition");
    }
    GLuint vertPosLoc = (GLuint) vertPosLocInt;

    // Vertex buffer array handling changes with OpenGL version, see
    // https://stackoverflow.com/a/21652955

    const float vertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };

    const GLubyte indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint vertexBuffer;
    GL_SAFECALL(glGenBuffers, 1, &vertexBuffer);
    GL_SAFECALL(glBindBuffer, GL_ARRAY_BUFFER, vertexBuffer);
    GL_SAFECALL(glBufferData, GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint vertexArray;
    GL_SAFECALL(glGenVertexArrays, 1, &vertexArray);
    GL_SAFECALL(glBindVertexArray, vertexArray);
    GL_SAFECALL(glVertexAttribPointer, vertPosLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    GL_SAFECALL(glEnableVertexAttribArray, vertPosLoc);

    GLuint indicesBuffer;
    GL_SAFECALL(glGenBuffers, 1, &indicesBuffer);
    GL_SAFECALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);
    GL_SAFECALL(glBufferData, GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // GL_SAFECALL(glEnableVertexAttribArray, vertPosLoc);
    // GL_SAFECALL(glBindBuffer, GL_ARRAY_BUFFER, vertexBuffer);
    // GL_SAFECALL(glVertexAttribPointer, vertPosLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    // GL_SAFECALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, indicesBuffer);

    GL_SAFECALL(glUseProgram, program);
    setUniformsJSON(program, params);

    GL_SAFECALL(glViewport, 0, 0, params.width, params.height);
    GL_SAFECALL(glClearColor, 0.0f, 0.0f, 0.0f, 1.0f);
    GL_SAFECALL(glClear, GL_COLOR_BUFFER_BIT);
    GL_SAFECALL(glDrawElements, GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    GL_SAFECALL(glFlush);
}

/*---------------------------------------------------------------------------*/
// PNG
/*---------------------------------------------------------------------------*/

// 4 channels: RGBA
#define CHANNELS (4)

/*---------------------------------------------------------------------------*/

void savePNG(Params& params) {
    unsigned int uwidth = (unsigned int) params.width;
    unsigned int uheight = (unsigned int) params.height;
    std::vector<std::uint8_t> data(uwidth * uheight * CHANNELS);
    GL_SAFECALL(glReadPixels, 0, 0, uwidth, uheight, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
    std::vector<std::uint8_t> flipped_data(uwidth * uheight * CHANNELS);
    for (unsigned int h = 0; h < uheight ; h++)
        for (unsigned int col = 0; col < uwidth * CHANNELS; col++)
            flipped_data[h * uwidth * CHANNELS + col] =
                data[(uheight - h - 1) * uwidth * CHANNELS + col];
    unsigned png_error = lodepng::encode(params.output, flipped_data, uwidth, uheight);
    if (png_error) {
        crash("lodepng: %s", lodepng_error_text(png_error));
    }
}

/*---------------------------------------------------------------------------*/
// Main
/*---------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
    std::string fragContents;
    Context context;
    Params params;

    setParams(params, argc, argv);
    readFile(fragContents, params.fragFilename);
    params.version = getVersion(fragContents);
    context_init(params, context);
    openglRender(params, fragContents);
    context_render(context);
    savePNG(params);
    context_terminate(context);
    exit(EXIT_SUCCESS);
}

/*---------------------------------------------------------------------------*/
