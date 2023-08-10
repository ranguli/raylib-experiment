#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"

void DrawHUD(Camera camera, int cameraMode) {
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
}