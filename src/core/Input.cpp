#include "core/Input.h"

void Input::Update() {
  hotbarPressed_ = -1;
  for (int i = 0; i < 9; ++i) {
    if (IsKeyPressed(KEY_ONE + i)) {
      hotbarPressed_ = i;
      break;
    }
  }
}

Vector2 Input::MoveAxis() const {
  Vector2 move = {0.0f, 0.0f};
  if (IsKeyDown(KEY_W)) move.y -= 1.0f;
  if (IsKeyDown(KEY_S)) move.y += 1.0f;
  if (IsKeyDown(KEY_A)) move.x -= 1.0f;
  if (IsKeyDown(KEY_D)) move.x += 1.0f;
  if (move.x == 0.0f && move.y == 0.0f) return move;
  return Vector2Normalize(move);
}

bool Input::MineHeld() const { return IsMouseButtonDown(MOUSE_BUTTON_LEFT); }
int Input::HotbarPressed() const { return hotbarPressed_; }
bool Input::ToggleInventoryPressed() const { return IsKeyPressed(KEY_TAB); }
bool Input::ToggleDebugPressed() const { return IsKeyPressed(KEY_GRAVE); }
bool Input::ToggleMinimapPressed() const { return IsKeyPressed(KEY_M); }
