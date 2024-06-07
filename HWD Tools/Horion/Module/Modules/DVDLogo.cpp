#include "DVDLogo.h"
#include "../../../DrawUtils.h"
#include "../../../../resource.h"

// Constants for logo dimensions and speed
const float LOGO_WIDTH = 142.f;
const float LOGO_HEIGHT = 69.f;
const float SPEED_X = 0.8f;
const float SPEED_Y = 1.f;

// Enum for different modes
enum DVDLogoMode {
    COLOR_CHANGE = 0,
    ROTATE = 1,
    IMAGE_CHANGE = 2
};

// Enum for selecting available images
enum AvailableImages {
    IMAGE_DVD = 0,
    IMAGE_ECHEST,
    IMAGE3,
    IMAGE4,
    IMAGE5
};

// List of available image resource names
const std::vector<const char*> IMAGE_NAMES = {
    "DVD",    // DVD logo
    "echest", // eChest image
    "image3", // Third image
    "image4", // Fourth image
    "image5"  // Fifth image
};

DVDLogo::DVDLogo() : IModule(0, Category::VISUAL, "DVD Screen Saver") {
    // Add mode entries
    mode.addEntry(EnumEntry("color change", DVDLogoMode::COLOR_CHANGE));
    mode.addEntry(EnumEntry("rotate", DVDLogoMode::ROTATE));
    mode.addEntry(EnumEntry("image change", DVDLogoMode::IMAGE_CHANGE));

    // Add image selection entries
    for (size_t i = 0; i < IMAGE_NAMES.size(); i++) {
        image1Selection.addEntry(EnumEntry(IMAGE_NAMES[i], static_cast<int>(i)));
        image2Selection.addEntry(EnumEntry(IMAGE_NAMES[i], static_cast<int>(i)));
    }

    // Load all available images into the textures map
    for (const char* imageName : IMAGE_NAMES) {
        textures[imageName] = DrawUtils::resourceToTexturePtr(imageName, imageName);
        // Check if the texture was loaded successfully
        if (!textures[imageName]) {
            std::cerr << "Failed to load texture for: " << imageName << std::endl;
        }
    }

    // Initialize current image to the first available image
    currentImage = textures["DVD"];
    selectedImage1 = currentImage;
    selectedImage2 = textures["echest"]; // Default to a different image for switching
}

DVDLogo::~DVDLogo() {
}

const char* DVDLogo::getModuleName() {
    return "DVDLogo";
}

void DVDLogo::onPostRender(MinecraftUIRenderContext* renderCtx) {
    auto player = Game.getLocalPlayer();
    if (!player) return;

    if (GameData::canUseMoveKeys()) {
        float height = Game.getGuiData()->heightGame;
        float width = Game.getGuiData()->widthGame;

        // Update position and check for boundary collision
        updatePosition(height, width);

        // Render the current image with rotation if enabled
        float currentRotationAngle = mode.isSelected(DVDLogoMode::ROTATE) ? rotationAngle : 0.f;
        DrawUtils::drawImageFromTexturePtr(currentImage, pos, Vec2(LOGO_WIDTH, LOGO_HEIGHT), Vec2(0.f, 0.f), Vec2(1.f, 1.f), currentRotationAngle);
        DrawUtils::flushImages(MC_Color(colorR, colorG, colorB));
    }
}

void DVDLogo::onEnable() {
    auto player = Game.getLocalPlayer();
    if (!player) return;

    // Center the logo on the screen
    pos.x = Game.getGuiData()->widthGame / 2 - LOGO_WIDTH / 2;
    pos.y = Game.getGuiData()->heightGame / 2 - LOGO_HEIGHT / 2;
    positiveX = true;
    positiveY = true;
    colorR = colorG = colorB = 255;
    rotationAngle = 0.f; // Initialize the rotation angle to 0 degrees

    // Set initial image based on user selection or default
    if (mode.isSelected(DVDLogoMode::IMAGE_CHANGE)) {
        selectUserImages();
        currentImage = selectedImage1; // Ensure currentImage is initialized to the first user-selected image
    } else {
        currentImage = textures["DVD"]; // Default to "DVD" image if IMAGE_CHANGE mode is not selected
    }
}

void DVDLogo::updatePosition(float screenHeight, float screenWidth) {
    bool hitWall = false; // Flag to check if a wall is hit

    // Vertical movement
    if (positiveY) {
        pos.y += SPEED_Y;
        if (pos.y + LOGO_HEIGHT > screenHeight) {
            if (mode.isSelected(DVDLogoMode::COLOR_CHANGE)) {
                changeColor();
            }
            positiveY = false;
            hitWall = true;
        }
    } else {
        pos.y -= SPEED_Y;
        if (pos.y < 0) {
            if (mode.isSelected(DVDLogoMode::COLOR_CHANGE)) {
                changeColor();
            }
            positiveY = true;
            hitWall = true;
        }
    }

    // Horizontal movement
    if (positiveX) {
        pos.x += SPEED_X;
        if (pos.x + LOGO_WIDTH > screenWidth) {
            if (mode.isSelected(DVDLogoMode::COLOR_CHANGE)) {
                changeColor();
            }
            positiveX = false;
            hitWall = true;
        }
    } else {
        pos.x -= SPEED_X;
        if (pos.x < 0) {
            if (mode.isSelected(DVDLogoMode::COLOR_CHANGE)) {
                changeColor();
            }
            positiveX = true;
            hitWall = true;
        }
    }

    // Rotate 90 degrees if a wall was hit and rotation is enabled
    if (hitWall) {
        if (mode.isSelected(DVDLogoMode::ROTATE)) {
            rotationAngle += 90.f;
            if (rotationAngle >= 360.f) {
                rotationAngle -= 360.f;
            }
        }
        // Switch images if image change is enabled
        if (mode.isSelected(DVDLogoMode::IMAGE_CHANGE)) {
            switchImage();
        }
    }
}

void DVDLogo::changeColor() {
    colorR = rand() % 127 + 127;
    colorG = rand() % 127 + 127;
    colorB = rand() % 127 + 127;
}

void DVDLogo::switchImage() {
    // Toggle between the user-selected images
    if (currentImage == selectedImage1) {
        currentImage = selectedImage2;
    } else {
        currentImage = selectedImage1;
    }
}

void DVDLogo::selectUserImages() {
    // Get user-selected images from the enum settings
    const char* image1Name = IMAGE_NAMES[image1Selection.getSelectedValue()];
    const char* image2Name = IMAGE_NAMES[image2Selection.getSelectedValue()];

    // Ensure that two different images are selected
    if (image1Name == image2Name) {
        // If the same image is selected for both, fallback to a default alternate image
        image2Name = (image1Name == "DVD") ? "echest" : "DVD";
    }

    selectedImage1 = textures[image1Name];
    selectedImage2 = textures[image2Name];

    // Check for texture validity and use fallback if needed
    if (!selectedImage1) {
        std::cerr << "Failed to load selectedImage1 texture for: " << image1Name << std::endl;
        selectedImage1 = textures["DVD"]; // Default to "DVD" if the image fails to load
    }
    if (!selectedImage2) {
        std::cerr << "Failed to load selectedImage2 texture for: " << image2Name << std::endl;
        selectedImage2 = textures["echest"]; // Default to "echest" if the image fails to load
    }
}
