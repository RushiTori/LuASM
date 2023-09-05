#ifndef MAIN_H
#define MAIN_H

#include "AppCommon.h"

static bool APP_HELPER = false;
static void printAppHelper() {
  APP_HELPER = true;
  printf("This is the helper message for \" %s \".\n", ProjectTitle);
  printf("\nList of common app args (dashes required):\n");
  printf("  -h OR -help : prints this helper message\n");
  printf("  -fullscreen : sets the app resolution to fullscreen\n");
  printf("  -resolution width height : sets the app resolution\n");
  printf("\nList of this app's args (dashes required):\n");
  printf("  No specific args for this app.\n");
}

static void setup(const string title, string* args, int argc) {
  ProjectTitle = title;
  int argIndex = 0;
  while (argIndex < argc) {
    if (strcmp(args[argIndex], "--help") == 0 ||
        strcmp(args[argIndex], "-help") == 0 ||
        strcmp(args[argIndex], "-h") == 0) {
      printAppHelper();
      argIndex++;
    } else {
      argIndex++;
    }
  }
  printf("Currently running : %s\n\n", ProjectTitle);
}

#endif  // MAIN_H
