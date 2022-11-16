#pragma once

#include "sui_def.h"

struct sui_context;
struct sui_window;

void sui_init();
void sui_terminate();
void sui_window_init();
void sui_window_terminate();

i32 sui_button();
void sui_label();
void sui_slider();
