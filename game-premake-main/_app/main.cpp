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
#include <vector>
#include <math.h>

using namespace std;

#pragma region Variables
float version = 0.0;
std::string bgMusic = "";
int num_textures;
float cameraHeight = -1.0;
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

Vector3** foregroundPosition;

struct Player {
    Vector3 position;
    Color color;
    int maxBombs;
    int num;
    bool dead;
};

bool gameOver = false;
bool hasPlayer1Won = false;
bool hasPlayer2Won = false;

struct Bombs {
    float pos_x;
    float pos_z;
    float timeToExplode;
};

std::vector <Player*> players;

std::vector <Bombs*> player1Bombs;
std::vector <Bombs*> player2Bombs;

std::vector <Vector3> powerUps;

#pragma endregion

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int Draw(void)
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
    std::cout << temp << std::endl;

    version = std::stof(temp);

    getline(level, temp);

#pragma endregion

#pragma region Titulo
    getline(level, temp, ';');
    cout << temp << std::endl;
    if (temp != "TITLE") {
        std::cout << "Titulo no encontrado" << std::endl;
        return 3;
    }

    getline(level, temp, ';');
    cout << temp << endl;
    if (temp == "")
    {
        std::cout << "El titulo esta vacio" << std::endl;
        return 4;
    }
    else
    {
        title = temp;
    }

    getline(level, temp);
    
#pragma endregion

#pragma region Musica
    if (version >= 0.5) {
        getline(level, temp, ';');
        cout << temp << endl;
        if (temp != "MUSIC") {
            cout << "Música no encontrada" << endl;
            return 4;
        }

        getline(level, temp, ';');
        cout << temp << endl;
        string extensionName = temp.substr(temp.find("."));
        if (extensionName != ".ogg") {
            cout << "La extension de la música no es correcta, necesitas .ogg" << endl;
            return 5;
        }
        bgMusic = temp;

        getline(level, temp);
    }
#pragma endregion

#pragma region Camara
    if (version >= 1.0) {
        getline(level, temp, ';');
        cout << temp << endl;
        if (temp != "CAMERA_HEIGHT") {
            cout << "Camara no encontrada" << endl;
            return 6;
        }

        getline(level, temp, ';');
        cout << temp << endl;
        cameraHeight = stof(temp);

        getline(level, temp);
    }
#pragma endregion
    if (version <= 0.21){
        getline(level, temp); 
        getline(level, temp);
    }
    else if (version <= 0.51) {
        getline(level, temp);
    }
#pragma region Texturas
    getline(level, temp, ';');
    cout << temp << endl;

    if (temp != "TEXTURES") {
        cout << "Texturas mal cargadas" << endl;
        return 7;
    }
    getline(level, temp, ';');
    cout << temp << endl;

    num_textures = stoi(temp);
    getline(level, temp);

#pragma endregion

#pragma region CargarMapa
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
        return 8;
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
        return 9;
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
        return 10;
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

void LoadTextures() {
    //Carga de texturas
    std::map<std::string, std::string>::iterator it;

    for (it = textures.begin(); it != textures.end(); it++)
    {
        std::cout << it->first << " : " << it->second << std::endl;
        const char* img_string = it->second.c_str();
        Image img = LoadImage(img_string);
        Texture2D levelTexture = LoadTextureFromImage(img);
        textures2D.insert(std::pair<std::string, Texture2D>(it->first.c_str(), levelTexture));
    }
}

bool Collisions(Vector3 pos) {
    for (size_t i = 0; i < objects_h; i++){
        for (size_t j = 0; j < objects_w; j++){
            if (foregroundPosition[i][j].x == pos.x && foregroundPosition[i][j].y == pos.y && foregroundPosition[i][j].z == pos.z) {
                if (objects[i][j] != "0" && objects[i][j] != "E") { return true; }
            }
        }
    }
    return false;
}

bool PowerUps(Vector3 pos) {
    for (size_t i = 0; i < objects_h; i++){
        for (size_t j = 0; j < objects_w; j++){
            if (foregroundPosition[i][j].x == pos.x && foregroundPosition[i][j].y == pos.y && foregroundPosition[i][j].z == pos.z) {
                if (objects[i][j] == "E") { return true; }
            }
        }
    }
    return false;
}

bool IsDestructible(Vector3 pos) {
    for (size_t i = 0; i < objects_h; i++)
    {
        for (size_t j = 0; j < objects_w; j++)
        {
            if (foregroundPosition[i][j].x == pos.x && foregroundPosition[i][j].y == pos.y && foregroundPosition[i][j].z == pos.z) {
                if (objects[i][j] == "D" || objects[i][j] == "W") { return true; }
            }
        }
    }
    return false;
}

void DestroyPowerUp(Vector3 pos) {
    for (size_t i = 0; i < objects_h; i++){
        for (size_t j = 0; j < objects_w; j++){
            if (foregroundPosition[i][j].x == pos.x && foregroundPosition[i][j].y == pos.y && foregroundPosition[i][j].z == pos.z){
                foreground[i][j] = "0";
                if (objects[i][j] == "W"){
                    objects[i][j] = "E";
                    Vector3 newPowerUp = { pos.x, 1, pos.z };
                    powerUps.push_back(newPowerUp);
                    return;
                }
                else if (objects[i][j] == "D"){
                    objects[i][j] = "0";
                    return;
                }
            }
        }
    }
}

void TakePowerUps(Vector3 pos) {
    for (size_t i = 0; i < objects_h; i++){
        for (size_t j = 0; j < objects_w; j++){
            if (foregroundPosition[i][j].x == pos.x && foregroundPosition[i][j].y == pos.y && foregroundPosition[i][j].z == pos.z){
                if (objects[i][j] == "E"){
                    objects[i][j] = "0";
                }
                if (powerUps.size() > 0) {
                    for (int i = 0; i < powerUps.size(); i++) {
                        if (powerUps[i].x == pos.x && powerUps[i].z == pos.z) {
                            powerUps.erase(powerUps.begin() + i);
                            return;
                        }
                    }
                }
            }
        }
    }
}

void RenderAndInputPlayer() {
    if (!gameOver)
    {
        for (int i = 0; i < players.size(); i++) {
            if (players[i]->num == 1) {
                if (IsKeyPressed(KEY_D) && !Collisions({ players[i]->position.x + 1, players[i]->position.y, players[i]->position.z })) {
                    players[i]->position = { players[i]->position.x + 1, players[i]->position.y, players[i]->position.z };
                    if (PowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        TakePowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }
                else if (IsKeyPressed(KEY_A) && !Collisions({ players[i]->position.x - 1, players[i]->position.y, players[i]->position.z })) {
                    players[i]->position = { players[i]->position.x - 1, players[i]->position.y, players[i]->position.z };
                    if (PowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        TakePowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }
                else if (IsKeyPressed(KEY_W) && !Collisions({ players[i]->position.x, players[i]->position.y, players[i]->position.z - 1 })) {
                    players[i]->position = { players[i]->position.x, players[i]->position.y, players[i]->position.z - 1 };
                    if (PowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        TakePowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }
                else if (IsKeyPressed(KEY_S) && !Collisions({ players[i]->position.x, players[i]->position.y, players[i]->position.z + 1 })) {
                    players[i]->position = { players[i]->position.x, players[i]->position.y, players[i]->position.z + 1 };
                    if (PowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        TakePowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }

                if (!players[i]->dead) {
                    DrawSphere(players[i]->position, 0.4f, players[i]->color);
                }
            }

            else if (players[i]->num == 2) {
                if (IsKeyPressed(KEY_RIGHT) && !Collisions({ players[i]->position.x + 1, players[i]->position.y, players[i]->position.z })) {
                    players[i]->position = { players[i]->position.x + 1, players[i]->position.y, players[i]->position.z };
                    if (PowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        TakePowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }
                else if (IsKeyPressed(KEY_LEFT) && !Collisions({ players[i]->position.x - 1, players[i]->position.y, players[i]->position.z })) {
                    players[i]->position = { players[i]->position.x - 1, players[i]->position.y, players[i]->position.z };
                    if (PowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        TakePowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }
                else if (IsKeyPressed(KEY_UP) && !Collisions({ players[i]->position.x, players[i]->position.y, players[i]->position.z - 1 })) {
                    players[i]->position = { players[i]->position.x, players[i]->position.y, players[i]->position.z - 1 };
                    if (PowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        TakePowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }
                else if (IsKeyPressed(KEY_DOWN) && !Collisions({ players[i]->position.x, players[i]->position.y, players[i]->position.z + 1 })) {
                    players[i]->position = { players[i]->position.x, players[i]->position.y, players[i]->position.z + 1 };
                    if (PowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        TakePowerUps({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }
                if (!players[i]->dead) {
                    DrawSphere(players[i]->position, 0.4f, players[i]->color);
                }
            }
        }
    }
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, title.c_str());

    Draw();
    LoadTextures();

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    //Comprobar la version para cambiar el tamaño de la camara
    if (version >= 1.0) {
        camera.position = { 0.0f, cameraHeight, 2.0f }; 
    }else {
        camera.position = { 0.0f, 20.0f, 2.0f };  // Camera position
    }
    camera.target = { 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type


    //Margins
    float x_margin = background_w % 2 == 0 ? .5 : 0;
    float z_margin = background_h % 2 == 0 ? .5 : 0;
    float x_offset = background_w / 2;
    float z_offset = background_h / 2;

    Vector3 cubePosition = { 0.0f, 2.0f, 0.0f };

    Vector3 initialPos = { -x_offset + x_margin, 0, -z_offset + z_margin };

    x_margin = foreground_w % 2 == 0 ? .5 : 0;
    z_margin = foreground_h % 2 == 0 ? .5 : 0;
    x_offset = foreground_w / 2;
    z_offset = foreground_h / 2;

    Vector3 fg_initialPos = { -x_offset + x_margin, 1, -z_offset + z_margin };
    Vector3 tempPos = initialPos;

    Vector3** positions = new Vector3 * [background_h];
    foregroundPosition = new Vector3 * [foreground_h];

    for (size_t i = 0; i < background_h; i++)
    {
        positions[i] = new Vector3[background_w];
        for (size_t j = 0; j < background_w; j++)
        {
            tempPos.x += 1.0f;
            positions[i][j] = tempPos;
        }
        tempPos.z += 1.0f;
        tempPos.x = initialPos.x;
    }

    tempPos = fg_initialPos;
    for (size_t i = 0; i < foreground_h; i++)
    {
        foregroundPosition[i] = new Vector3[foreground_w];
        for (size_t j = 0; j < foreground_w; j++)
        {
            tempPos.x += 1.0f;
            foregroundPosition[i][j] = tempPos;
        }
        tempPos.z += 1.0f;
        tempPos.x = fg_initialPos.x;
    }

    for (size_t i = 0; i < objects_h; i++)
    {
        for (size_t j = 0; j < objects_w; j++)
        {
            if (objects[i][j] == "1") {
                objects[i][j] = "0";
                Player* newPlayer = new Player;
                newPlayer->position = foregroundPosition[i][j];
                newPlayer->color = BLUE;
                newPlayer->maxBombs = 1;
                newPlayer->num = 1;
                newPlayer->dead = false;
                players.push_back(newPlayer);
            }
            else if (objects[i][j] == "2") {
                objects[i][j] = "0";
                Player* newPlayer = new Player;
                newPlayer->position = foregroundPosition[i][j];
                newPlayer->color = PURPLE;
                newPlayer->maxBombs = 1;
                newPlayer->num = 2;
                newPlayer->dead = false;
                players.push_back(newPlayer);
            }
        }
    }

    Music bgMusic2;
    if (version >= 0.5) {
        InitAudioDevice();
        bgMusic2 = LoadMusicStream(bgMusic.c_str());
        bgMusic2.looping = true;
        PlayMusicStream(bgMusic2);
    }

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();
        if (version >= 0.5) {
            UpdateMusicStream(bgMusic2);
        }

        if (!gameOver)
        {
            //Agregamos el input de las bombas, y el tiempo para que exploten
            if (IsKeyPressed(KEY_SPACE) && players[0]->maxBombs > player1Bombs.size()) {
                Bombs* newBomb = new Bombs;
                newBomb->pos_x = players[0]->position.x;
                newBomb->pos_z = players[0]->position.z;
                newBomb->timeToExplode = 2.0f;
                player1Bombs.push_back(newBomb);
            }

            if (IsKeyPressed(KEY_ENTER) && players[1]->maxBombs > player2Bombs.size()) {
                Bombs* newBomb = new Bombs;
                newBomb->pos_x = players[1]->position.x;
                newBomb->pos_z = players[1]->position.z;
                newBomb->timeToExplode = 2.0f;
                player2Bombs.push_back(newBomb);
            }
        }

        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        //Renderizado background
        for (size_t i = 0; i < background_h; i++)
        {
            for (size_t j = 0; j < background_w; j++)
            {
                if (background[i][j] == "C") {
                    std::string t = background[i][j];
                    DrawCubeTexture(textures2D[t], positions[i][j], 1.0f, 1.0f, 1.0f, WHITE);
                }
                if (background[i][j] == "V") {
                    std::string t = background[i][j];
                    DrawCubeTexture(textures2D[t], positions[i][j], 1.0f, 1.0f, 1.0f, WHITE);
                }
            }
        }

        //Renderizado foreground
        for (size_t i = 0; i < foreground_h; i++)
        {
            for (size_t j = 0; j < foreground_w; j++)
            {
                if (foreground[i][j] == "P") {
                    std::string t = foreground[i][j];
                    DrawCubeTexture(textures2D[t], foregroundPosition[i][j], 1.0f, 1.0f, 1.0f, WHITE);
                }
                else if (foreground[i][j] == "T") {
                    std::string t = foreground[i][j];
                    DrawCubeTexture(textures2D[t], foregroundPosition[i][j], 1.0f, 1.0f, 1.0f, WHITE);
                }
            }
        }

        //Renderizamos el jugador y el input
        RenderAndInputPlayer();

        //Bombas
        if (player1Bombs.size() > 0) {
            for (int i = 0; i < player1Bombs.size(); i++) {
                player1Bombs[i]->timeToExplode -= deltaTime;
                if (player1Bombs[i]->timeToExplode > 0) {
                    DrawSphere({ player1Bombs[i]->pos_x, 1.0f, player1Bombs[i]->pos_z }, 0.1 + 0.2f * abs(sin(player1Bombs[i]->timeToExplode * 2)), BLACK);
                }
                else if (player1Bombs[i]->timeToExplode < 0) {
                    //Miramos las 4 direcciones si hay un destructible en objects
                    if (IsDestructible({ player1Bombs[i]->pos_x + 1, 1, player1Bombs[i]->pos_z })) {
                        DestroyPowerUp({ player1Bombs[i]->pos_x + 1, 1, player1Bombs[i]->pos_z });
                    }
                    else if (IsDestructible({ player1Bombs[i]->pos_x - 1, 1, player1Bombs[i]->pos_z })) {
                        if (PowerUps({ player1Bombs[i]->pos_x - 1, 1, player1Bombs[i]->pos_z })) {
                            Vector3 newPowerUp = { player1Bombs[i]->pos_x - 1, 1, player1Bombs[i]->pos_z };
                            powerUps.push_back(newPowerUp);
                        }
                        DestroyPowerUp({ player1Bombs[i]->pos_x - 1, 1, player1Bombs[i]->pos_z });
                    }
                    else if (IsDestructible({ player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z + 1 })) {
                        if (PowerUps({ player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z + 1 })) {
                            Vector3 newPowerUp = { player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z + 1 };
                            powerUps.push_back(newPowerUp);
                        }
                        DestroyPowerUp({ player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z + 1 });
                    }
                    else if (IsDestructible({ player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z - 1 })) {
                        if (PowerUps({ player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z - 1 })) {
                            Vector3 newPowerUp = { player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z - 1 };
                            powerUps.push_back(newPowerUp);
                        }
                        DestroyPowerUp({ player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z - 1 });
                    }
                    //Miramos si hay players
                    if (player1Bombs[i]->pos_x == players[0]->position.x && player1Bombs[i]->pos_z == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player1Bombs[i]->pos_x == players[1]->position.x && player1Bombs[i]->pos_z == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player1Bombs[i]->pos_x + 1 == players[0]->position.x && player1Bombs[i]->pos_z == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player1Bombs[i]->pos_x + 1 == players[1]->position.x && player1Bombs[i]->pos_z == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player1Bombs[i]->pos_x - 1 == players[0]->position.x && player1Bombs[i]->pos_z == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player1Bombs[i]->pos_x - 1 == players[1]->position.x && player1Bombs[i]->pos_z == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player1Bombs[i]->pos_x == players[0]->position.x && player1Bombs[i]->pos_z + 1 == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player1Bombs[i]->pos_x == players[1]->position.x && player1Bombs[i]->pos_z + 1 == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player1Bombs[i]->pos_x == players[0]->position.x && player1Bombs[i]->pos_z - 1 == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player1Bombs[i]->pos_x == players[1]->position.x && player1Bombs[i]->pos_z - 1 == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    player1Bombs.erase(player1Bombs.begin() + i);
                }
            }
        }
        if (player2Bombs.size() > 0) {
            for (int i = 0; i < player2Bombs.size(); i++)
            {
                player2Bombs[i]->timeToExplode -= deltaTime;
                if (player2Bombs[i]->timeToExplode > 0) {
                    DrawSphere({ player2Bombs[i]->pos_x, 1.0f, player2Bombs[i]->pos_z }, 0.1 + 0.2f * abs(sin(player2Bombs[i]->timeToExplode * 2)), BLACK);
                }
                else if (player2Bombs[i]->timeToExplode < 0) {
                    //Miramos las 4 direcciones los destructibles
                    if (IsDestructible({ player2Bombs[i]->pos_x + 1, 1, player2Bombs[i]->pos_z })) {
                        if (PowerUps({ player2Bombs[i]->pos_x + 1, 1, player2Bombs[i]->pos_z })) {
                            Vector3 newPowerUp = { player2Bombs[i]->pos_x + 1, 1, player2Bombs[i]->pos_z };
                            powerUps.push_back(newPowerUp);
                        }
                        DestroyPowerUp({ player2Bombs[i]->pos_x + 1, 1, player2Bombs[i]->pos_z });
                    }
                    else if (IsDestructible({ player2Bombs[i]->pos_x - 1, 1, player2Bombs[i]->pos_z })) {
                        if (PowerUps({ player2Bombs[i]->pos_x - 1, 1, player2Bombs[i]->pos_z })) {
                            Vector3 newPowerUp = { player2Bombs[i]->pos_x - 1, 1, player2Bombs[i]->pos_z };
                            powerUps.push_back(newPowerUp);
                        }
                        DestroyPowerUp({ player2Bombs[i]->pos_x - 1, 1, player2Bombs[i]->pos_z });
                    }
                    else if (IsDestructible({ player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z + 1 })) {
                        if (PowerUps({ player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z + 1 })) {
                            Vector3 newPowerUp = { player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z + 1 };
                            powerUps.push_back(newPowerUp);
                        }
                        DestroyPowerUp({ player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z + 1 });
                    }
                    else if (IsDestructible({ player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z - 1 })) {
                        if (PowerUps({ player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z - 1 })) {
                            Vector3 newPowerUp = { player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z - 1 };
                            powerUps.push_back(newPowerUp);
                        }
                        DestroyPowerUp({ player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z - 1 });
                    }
                    //Miramos si hay players
                    if (player2Bombs[i]->pos_x == players[0]->position.x && player2Bombs[i]->pos_z == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player2Bombs[i]->pos_x == players[1]->position.x && player2Bombs[i]->pos_z == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player2Bombs[i]->pos_x + 1 == players[0]->position.x && player2Bombs[i]->pos_z == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player2Bombs[i]->pos_x + 1 == players[1]->position.x && player2Bombs[i]->pos_z == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player2Bombs[i]->pos_x - 1 == players[0]->position.x && player2Bombs[i]->pos_z == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player2Bombs[i]->pos_x - 1 == players[1]->position.x && player2Bombs[i]->pos_z == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player2Bombs[i]->pos_x == players[0]->position.x && player2Bombs[i]->pos_z + 1 == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player2Bombs[i]->pos_x == players[1]->position.x && player2Bombs[i]->pos_z + 1 == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player2Bombs[i]->pos_x == players[0]->position.x && player2Bombs[i]->pos_z - 1 == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player2Bombs[i]->pos_x == players[1]->position.x && player2Bombs[i]->pos_z - 1 == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    player2Bombs.erase(player2Bombs.begin() + i);
                }
            }
        }

        //Dibujamos los Power Ups
        if (powerUps.size() > 0){
            for (int i = 0; i < powerUps.size(); i++){
                DrawCube(powerUps[i], 0.4f, 0.4f, 0.4f, YELLOW);
            }
        }

        if (!gameOver) { ClearBackground(RAYWHITE); }
        else { ClearBackground(DARKGRAY); }

        EndMode3D();

        //Condicion de derrota
        if (!gameOver) {
            if (players[1]->dead) { hasPlayer1Won = true; gameOver = true;}
            else if (players[0]->dead) { hasPlayer2Won = true; gameOver = true; }
        }

        if (hasPlayer1Won) { 
            DrawText("Game Over!", screenWidth / 2.85f, screenHeight / 3.0f, 80, ORANGE);
            string winText = "El ganador es el jugador de azul";
            DrawText(winText.c_str(), screenWidth / 3.5f, screenHeight / 2.0f, 50, ORANGE);
            DrawText("Presiona [ENTER] para salir", screenWidth / 2.65f, screenHeight / 1.6f, 30, BLACK);
            if (IsKeyPressed(KEY_ENTER)) { exit(0); }
        }
        if (hasPlayer2Won) { 
            DrawText("Game Over!", screenWidth / 2.85f, screenHeight / 3.0f, 80, ORANGE);
            string winText = "El ganador es el jugador de purpura";
            DrawText(winText.c_str(), screenWidth / 3.5f, screenHeight / 2.0f, 50, ORANGE);
            DrawText("Presiona [ENTER] para salir", screenWidth / 2.65f, screenHeight / 1.6f, 30, BLACK);
            if (IsKeyPressed(KEY_ENTER)) { exit(0); }
        }


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