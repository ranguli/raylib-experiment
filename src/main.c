/*******************************************************************************************
 *
 *   raylib [core] example - 3d camera first person
 *
 *   Example originally created with raylib 1.3, last time updated with
 *raylib 1.3
 *
 *   Example licensed under an unmodified zlib/libpng license, which is an
 *OSI-certified, BSD-like license that allows static linking with closed source
 *software
 *
 *   Copyright (c) 2015-2023 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

#define MAX_COLUMNS 20

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------

  // Fullscreen: setting the correct resolution
  // int display = GetCurrentMonitor();
  // const int screenWidth = GetMonitorWidth(display);
  // const int screenHeight = GetMonitorHeight(display);

  const int screenWidth = 1366;
  const int screenHeight = 768;

  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT); // Enable VSYNC and 4x Antialiasing if supported
  InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");

  // Fullscreen: enable
  // ToggleFullscreen();

  // Define the camera to look into our 3d world (position, target, up vector)
  Camera camera = {0};
  camera.position = (Vector3){0.0f, 2.0f, 4.0f}; // Camera position
  camera.target = (Vector3){0.0f, 2.0f, 0.0f};   // Camera looking at point
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};       // Camera up vector (rotation towards target)
  camera.fovy = 60.0f;                           // Camera field-of-view Y
  camera.projection = CAMERA_PERSPECTIVE;        // Camera projection type

  int cameraMode = CAMERA_FIRST_PERSON;

  // Generates some random columns
  float heights[MAX_COLUMNS] = {0};
  Vector3 positions[MAX_COLUMNS] = {0};
  Color colors[MAX_COLUMNS] = {0};

  for (int i = 0; i < MAX_COLUMNS; i++) {
    heights[i] = (float)GetRandomValue(1, 12);
    positions[i] =
        (Vector3){(float)GetRandomValue(-15, 15), heights[i] / 2.0f, (float)GetRandomValue(-15, 15)};
    colors[i] = (Color){GetRandomValue(20, 255), GetRandomValue(10, 55), 30, 255};
  }

  // Load plane model from a generated mesh
  Model plane = LoadModelFromMesh(GenMeshPlane(32.0f, 32.0f, 3, 3));

  /** Load basic lighting shaders. I have no god damn clue how shaders work, we're just importing the stuff
   * from the raylib example. The only difference is that I'm just using the shaders for OpenGL 3.30 and not
   * bothering with the dynamic support for other versions.
   *
   * NOTE: Raylib seems to need absolute filepaths in order to load stuff. Right now we're using a macro
   * defined in CMake.
   *
   * TODO: In the future, the game would have a folder structure once its installed, and there
   * would be a directory where we expect the shaders to be.
   */

  Shader shader = LoadShader(SHADER_PATH "lighting.vs", SHADER_PATH "lighting.fs");

  /** Get some required shader locations.
   * NOTE: "viewPos" is something defined in lighting.fs, the fragment shader.
   * TODO: what is a shader location?
   */
  shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

  /** NOTE: "matModel" location name is automatically assigned on shader loading,
  /* no need to get the location again if using that uniform name
  /* shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader,
  /* "matModel");
  */

  /** Ambient light level (some basic lighting)
   * NOTE: We're getting another shader location here called "ambient" which
   * like "viewPos" is defined in the fragment shader.
   */
  int ambientLoc = GetShaderLocation(shader, "ambient");
  SetShaderValue(shader, ambientLoc, (float[4]){0.1f, 0.1f, 0.1f, 1.0f}, SHADER_UNIFORM_VEC4);

  // Assign out lighting shader to model
  plane.materials[0].shader = shader;
  // cube.materials[0].shader = shader;

  // Create a single light for now.
  Light lights[MAX_LIGHTS] = {0};
  lights[0] = CreateLight(LIGHT_POINT, (Vector3){-2, 1.75, -2}, Vector3Zero(), WHITE, shader);

  DisableCursor(); // Limit cursor to relative movement inside the window
  // SetTargetFPS(60); // Set our game to run at 60 frames-per-second

  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {

    // Update
    //----------------------------------------------------------------------------------
    // Switch camera mode to allow "rolling" with the mouse
    if (IsKeyPressed(KEY_ONE)) {
      cameraMode = CAMERA_FREE;
      camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // Reset roll
    }

    if (IsKeyPressed(KEY_TWO)) {
      cameraMode = CAMERA_FIRST_PERSON;
      camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // Reset roll
    }

    if (IsKeyPressed(KEY_THREE)) {
      cameraMode = CAMERA_THIRD_PERSON;
      camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // Reset roll
    }

    if (IsKeyPressed(KEY_FOUR)) {
      cameraMode = CAMERA_ORBITAL;
      camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // Reset roll
    }

    // Switch camera projection
    if (IsKeyPressed(KEY_P)) {
      if (camera.projection == CAMERA_PERSPECTIVE) {
        // Create isometric view
        cameraMode = CAMERA_THIRD_PERSON;
        // Note: The target distance is related to the render distance in the
        // orthographic projection
        camera.position = (Vector3){0.0f, 2.0f, -100.0f};
        camera.target = (Vector3){0.0f, 2.0f, 0.0f};
        camera.up = (Vector3){0.0f, 1.0f, 0.0f};
        camera.projection = CAMERA_ORTHOGRAPHIC;
        camera.fovy = 20.0f; // near plane width in CAMERA_ORTHOGRAPHIC
        CameraYaw(&camera, -135 * DEG2RAD, true);
        CameraPitch(&camera, -45 * DEG2RAD, true, true, false);
      } else if (camera.projection == CAMERA_ORTHOGRAPHIC) {
        // Reset to default view
        cameraMode = CAMERA_THIRD_PERSON;
        camera.position = (Vector3){0.0f, 2.0f, 10.0f};
        camera.target = (Vector3){0.0f, 2.0f, 0.0f};
        camera.up = (Vector3){0.0f, 1.0f, 0.0f};
        camera.projection = CAMERA_PERSPECTIVE;
        camera.fovy = 60.0f;
      }
    }

    UpdateCamera(&camera, cameraMode); // Update camera
    //----------------------------------------------------------------------------------

    // Update the shader with the camera view vector (points towards { 0.0f,
    // 0.0f, 0.0f })
    float cameraPos[3] = {camera.position.x, camera.position.y, camera.position.z};
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
    // Check key inputs to enable/disable lights
    if (IsKeyPressed(KEY_Y)) {
      lights[0].enabled = !lights[0].enabled;
    }

    // Update light values (actually, only enable/disable them)
    for (int i = 0; i < MAX_LIGHTS; i++)
      UpdateLightValues(shader, lights[i]);

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    // Begin 3D drawing
    // ---------------------------------------------------------------------------------
    BeginMode3D(camera);

    // Instead of drawing a 'Plane', defined at draw time, with DrawPlane like this:
    // DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){32.0f, 32.0f}, LIGHTGRAY);
    // We've already defined it as a 'Model' (which has its own mesh) earlier in the code and are now drawing
    // it.

    // Draw our plane model in white at position (0,0,0) with a scale of 1.0
    DrawModel(plane, Vector3Zero(), 1.0f, LIGHTGRAY);

    DrawCube((Vector3){-16.0f, 2.5f, 0.0f}, 1.0f, 5.0f, 32.0f,
             BLUE); // Draw a blue wall
    DrawCube((Vector3){16.0f, 2.5f, 0.0f}, 1.0f, 5.0f, 32.0f,
             LIME); // Draw a green wall
    DrawCube((Vector3){0.0f, 2.5f, 16.0f}, 32.0f, 5.0f, 1.0f,
             GOLD); // Draw a yellow wall

    // Draw some cubes around
    for (int i = 0; i < MAX_COLUMNS; i++) {
      DrawCube(positions[i], 2.0f, heights[i], 2.0f, colors[i]);
      DrawCubeWires(positions[i], 2.0f, heights[i], 2.0f, MAROON);
    }

    // Draw player cube
    if (cameraMode == CAMERA_THIRD_PERSON) {
      DrawCube(camera.target, 0.5f, 0.5f, 0.5f, PURPLE);
      DrawCubeWires(camera.target, 0.5f, 0.5f, 0.5f, DARKPURPLE);
    }

    // Draw spheres to show where the lights are
    for (int i = 0; i < MAX_LIGHTS; i++) {
      if (lights[i].enabled)
        DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
      else
        DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
    }

    EndMode3D();

    // End 3D rendering, back to 2D rendering
    // ------------------------------------------------------------------------

    // Draw info boxes
    DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(5, 5, 330, 100, BLUE);

    DrawText("Camera controls:", 15, 15, 10, BLACK);
    DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
    DrawText("- Look around: arrow keys or mouse", 15, 45, 10, BLACK);
    DrawText("- Camera mode keys: 1, 2, 3, 4", 15, 60, 10, BLACK);
    DrawText("- Zoom keys: num-plus, num-minus or mouse scroll", 15, 75, 10, BLACK);
    DrawText("- Camera projection key: P", 15, 90, 10, BLACK);

    DrawRectangle(600, 5, 195, 100, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(600, 5, 195, 100, BLUE);

    DrawText("Camera status:", 610, 15, 10, BLACK);
    DrawText(TextFormat("- Mode: %s", (cameraMode == CAMERA_FREE)           ? "FREE"
                                      : (cameraMode == CAMERA_FIRST_PERSON) ? "FIRST_PERSON"
                                      : (cameraMode == CAMERA_THIRD_PERSON) ? "THIRD_PERSON"
                                      : (cameraMode == CAMERA_ORBITAL)      ? "ORBITAL"
                                                                            : "CUSTOM"),
             610, 30, 10, BLACK);
    DrawText(TextFormat("- Projection: %s", (camera.projection == CAMERA_PERSPECTIVE)    ? "PERSPECTIVE"
                                            : (camera.projection == CAMERA_ORTHOGRAPHIC) ? "ORTHOGRAPHIC"
                                                                                         : "CUSTOM"),
             610, 45, 10, BLACK);
    DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y,
                        camera.position.z),
             610, 60, 10, BLACK);
    DrawText(
        TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z),
        610, 75, 10, BLACK);
    DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10,
             BLACK);

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------

  // Now that our plane is a model, it needs to be unloaded
  UnloadModel(plane);

  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
