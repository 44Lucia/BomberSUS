/*******************************************************************************************
*
*   raylib [core] example - Initialize 3d camera mode
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

#pragma region Variables
float version = 0.0;
int num_textures;
string title;
map<string, string> textures;

map<string, Texture2D> textures2D;

string** background;
int background_w;
int background_h;

string** foreground;
int foreground_w;
int foreground_h;

string** objects;
int objects_w;
int objects_h;

float cubeSize[3] = { 2.0f,2.0f,2.0f };
#pragma endregion

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int draw(void)
{

#pragma region AbrirArchivo
    string temp;

    fstream level("level.sus");

    if (!level.is_open())
    {
        cout << "Error al abrir el archivo";
        return 1;
    }

    getline(level, temp, ';');
#pragma endregion

#pragma region CabecerayVersion
    cout << temp << endl;
    if (temp != "SUS") {
        cout << "Error de cabecera";
        return 2;
    }
    getline(level, temp, ';');
    version = stof(temp);
    cout << version << endl;

    getline(level, temp);
    getline(level, temp, ';');

#pragma endregion

#pragma region Titulo
    if (temp != "TITLE") {
        cout << "Titulo mal formado\n";
        return 3;
    }
    else {

        title = temp;
    }
    cout << title << endl;

    getline(level, temp);
    getline(level, temp, ';');
#pragma endregion

#pragma region Texturas
    if (temp != "TEXTURES") {
        cout << "Texturas mal cargadas\n";
        return 4;
    }
    getline(level, temp, ';');


    num_textures = stoi(temp);
    cout << temp << endl;

#pragma endregion

#pragma region CargarMapa
    getline(level, temp);
    for (int i = 0; i < num_textures; i++)
    {
        string key;
        getline(level, key, ';');

        string value;
        getline(level, value, ';');

        textures[key] = value;

        cout << key << ": " << value << endl;
        getline(level, temp);



    }
    getline(level, temp, ';');
    cout << temp << endl;


#pragma endregion

#pragma region CargarBackground

    if (temp != "BACKGROUND") {
        cout << "Background mal formado" << endl;
        return 5;
    }

    getline(level, temp, ';');
    cout << temp << endl;
    background_w = stoi(temp);

    getline(level, temp, ';');
    cout << temp << endl;
    background_h = stoi(temp);

    getline(level, temp);

    background = new string * [background_h];
    for (int i = 0; i < background_h; i++) {
        background[i] = new string[background_w];
    }

    for (int j = 0; j < background_h; j++) {
        for (int i = 0; i < background_w; i++) {
            getline(level, temp, ';');
            cout << temp;

            background[j][i] = temp;
        }

        cout << endl;
        getline(level, temp);
    }
    getline(level, temp, ';');
    cout << temp << endl;
#pragma endregion

#pragma region CargarForeground
    if (temp != "FOREGROUND") {
        cout << "Foreground mal formado" << endl;
        return 6;
    }

    getline(level, temp, ';');
    cout << temp << endl;
    foreground_w = stoi(temp);

    getline(level, temp, ';');
    cout << temp << endl;
    foreground_h = stoi(temp);

    getline(level, temp);

    foreground = new string * [foreground_h];
    for (int i = 0; i < foreground_h; i++) {
        foreground[i] = new string[foreground_w];
    }

    for (int j = 0; j < foreground_h; j++) {
        for (int i = 0; i < foreground_w; i++) {
            getline(level, temp, ';');
            cout << temp;

            foreground[j][i] = temp;
        }

        cout << endl;
        getline(level, temp);
    }
    getline(level, temp, ';');
    cout << temp << endl;

#pragma endregion

#pragma region CargarObjects
    if (temp != "OBJECTS") {
        cout << "Objecto mal formado" << endl;
        return 7;
    }

    getline(level, temp, ';');
    cout << temp << endl;
    objects_w = stoi(temp);

    getline(level, temp, ';');
    cout << temp << endl;
    objects_h = stoi(temp);

    getline(level, temp);

    objects = new string * [objects_h];
    for (int i = 0; i < foreground_h; i++) {
        objects[i] = new string[objects_w];
    }

    for (int j = 0; j < objects_h; j++) {
        for (int i = 0; i < objects_w; i++) {
            getline(level, temp, ';');
            cout << temp;

            objects[j][i] = temp;
        }

        cout << endl;
        getline(level, temp);
    }
    getline(level, temp, ';');
    cout << temp << endl;

#pragma endregion

}

void loadTextures() {

    map<string, string>::iterator it;

    cout << "TEXTURAS CARGADAS CORRECTAMENTE: " << endl;

    for (it = textures.begin(); it != textures.end(); it++){
        
        cout << "\n" << it->first << " " << it->second << endl;
        
        textures2D[it->first] = LoadTextureFromImage(LoadImage(it->second.c_str()));
    }
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera mode");

    draw();

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = { 0.0f, 10.0f, 10.0f };  // Camera position
    camera.target = { 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type

    Vector3 cubePosition = { 0.0f, 2.0f, 0.0f };


    loadTextures();
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        //Draw Background
        for (int i = 0; i < background_h; i++) {
            for (int j = 0; j < background_w; j++) {
                string t = background[i][j];
                cubePosition = { (float)j * 2 - background_h ,0.0f, (float)i * 2 - background_w };
                DrawCubeTexture(textures2D[t], cubePosition, cubeSize[0], cubeSize[1], cubeSize[2], GREEN); 
                DrawCubeWires(cubePosition, cubeSize[0], cubeSize[1], cubeSize[2], MAROON);
            }
        }
        //Draw Foreground
        for (int i = 0; i < foreground_h; i++) {
            for (int j = 0; j < foreground_w; j++) {
                string t = foreground[i][j];
                cubePosition = { (float)j * 2 - foreground_h ,2.0f, (float)i * 2 - foreground_w };
                if (foreground[i][j] != "0")
                {
                    DrawCubeTexture(textures2D[t], cubePosition, cubeSize[0], cubeSize[1], cubeSize[2], GRAY);
                }
            }
        }

        DrawGrid(10, 1.0f);

        EndMode3D();

        DrawText("BomberSUS", 10, 40, 20, DARKGRAY);

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}