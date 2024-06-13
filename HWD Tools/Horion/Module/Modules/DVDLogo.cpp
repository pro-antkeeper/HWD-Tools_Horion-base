#include "DVDLogo.h"

#include "../../../../resource.h"
#include "../../../DrawUtils.h"

const float LOGO_WIDTH = 69.f;
const float LOGO_HEIGHT = 69.f;
const float SPEED_X = 0.8f;
const float SPEED_Y = 1.f;

enum DVDLogoMode {
	COLOR_CHANGE = 0,
//	ROTATE = 1,
	IMAGE_CHANGE = 1
};

// Map of available image resource names to corresponding data
const std::unordered_map<int, const char*> IMAGE_DATA_MAP = {
	{0, "DVD_DATA"},
	{1, "LOGO_DATA"},
	{2, "HORION_BANNER_DATA"},
	{3, "CHECK_DATA"},
	{4, "CROSS_DATA"}};

DVDLogo::DVDLogo() : IModule(0, Category::VISUAL, "Epic HWD Screen Saver(rotation someday)") {
	registerEnumSetting("Mode", &mode, 2);
	mode.addEntry(EnumEntry("Color change", COLOR_CHANGE))
	//	.addEntry(EnumEntry("Rotate", ROTATE))
		.addEntry(EnumEntry("Image change", IMAGE_CHANGE));

	registerEnumSetting("Image #1", &image1Selection, 0);
	image1Selection.addEntry(EnumEntry("DVD", 0))
		.addEntry(EnumEntry("LOGO", 1))
		.addEntry(EnumEntry("HORION BANNER", 2))
		.addEntry(EnumEntry("CHECK", 3))
		.addEntry(EnumEntry("CROSS", 4));

	registerEnumSetting("Image #2", &image2Selection, 1);
	image2Selection.addEntry(EnumEntry("DVD", 0))
		.addEntry(EnumEntry("LOGO", 1))
		.addEntry(EnumEntry("HORION BANNER", 2))
		.addEntry(EnumEntry("CHECK", 3))
		.addEntry(EnumEntry("CROSS", 4));

	selectUserImages();  // Initialize selected images
}

DVDLogo::~DVDLogo() {
}

const char* DVDLogo::getModuleName() {
	return "ScreenSaver!";
}

void DVDLogo::onPostRender(MinecraftUIRenderContext* renderCtx) {
	auto player = Game.getLocalPlayer();
	if (!player) return;

	if (GameData::canUseMoveKeys()) {
		float height = Game.getGuiData()->heightGame;
		float width = Game.getGuiData()->widthGame;

		// Update position and check for boundary collision
		updatePosition(height, width);
		int num = std::stoi(currentImage);

		//float currentRotationAngle = (mode.selected == ROTATE) ? rotationAngle : 0.f;
		auto texturePtr = DrawUtils::resourceToTexturePtr(num, "currentImage");                                    
		DrawUtils::drawImageFromTexturePtr(texturePtr, pos, Vec2(LOGO_WIDTH, LOGO_HEIGHT), Vec2(0.f, 0.f), Vec2(1.f, 1.f));   //currentRotationAngle,
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
//	rotationAngle = 0.f;  // Initialize the rotation angle to 0 degrees
	selectUserImages();   // Ensure selected images are loaded
}

void DVDLogo::updatePosition(float screenHeight, float screenWidth) {
	bool hitWall = false;  // Flag to check if a wall is hit

	// Vertical movement
	if (positiveY) {
		pos.y += SPEED_Y;
		if (pos.y + LOGO_HEIGHT > screenHeight) {
			if (mode.selected == COLOR_CHANGE) {
				changeColor();
			}
			positiveY = false;
			hitWall = true;
		}
	} else {
		pos.y -= SPEED_Y;
		if (pos.y < 0) {
			if (mode.selected == COLOR_CHANGE) {
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
			if (mode.selected == COLOR_CHANGE) {
				changeColor();
			}
			positiveX = false;
			hitWall = true;
		}
	} else {
		pos.x -= SPEED_X;
		if (pos.x < 0) {
			if (mode.selected == COLOR_CHANGE) {
				changeColor();
			}
			positiveX = true;
			hitWall = true;
		}
	}

	// Rotate 90 degrees if a wall was hit and rotation is enabled
	if (hitWall) {
		/* if (mode.selected == ROTATE) {
			rotationAngle += 90.f;
			if (rotationAngle >= 360.f) {
				rotationAngle -= 360.f;
			}
		}*/
		// Switch images if image change is enabled
		if (mode.selected == IMAGE_CHANGE) {
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
	if (currentImage == IMAGE_DATA1) {
		currentImage = IMAGE_DATA2;
	} else {
		currentImage = IMAGE_DATA1;
	}
}

void DVDLogo::selectUserImages() {
	// Assign IMAGE_DATA1 based on image1Selection
	if (image1Selection.selected == 0) {
		IMAGE_DATA1 = "DVD_DATA";
	} else if (image1Selection.selected == 1) {
		IMAGE_DATA1 = "LOGO_DATA";
	} else if (image1Selection.selected == 2) {
		IMAGE_DATA1 = "HORION_BANNER_DATA";
	} else if (image1Selection.selected == 3) {
		IMAGE_DATA1 = "CHECK_DATA";
	} else if (image1Selection.selected == 4) {
		IMAGE_DATA1 = "CROSS_DATA";
	}

	// Assign IMAGE_DATA2 based on image2Selection
	if (image2Selection.selected == 0) {
		IMAGE_DATA2 = "DVD_DATA";
	} else if (image2Selection.selected == 1) {
		IMAGE_DATA2 = "LOGO_DATA";
	} else if (image2Selection.selected == 2) {
		IMAGE_DATA2 = "HORION_BANNER_DATA";
	} else if (image2Selection.selected == 3) {
		IMAGE_DATA2 = "CHECK_DATA";
	} else if (image2Selection.selected == 4) {
		IMAGE_DATA2 = "CROSS_DATA";
	}

	// Initialize the current image to the first selected image
	currentImage = IMAGE_DATA1;
}

std::string DVDLogo::getImageData(int selection) {
	// Return the corresponding image data name based on the selection
	auto it = IMAGE_DATA_MAP.find(selection);
	if (it != IMAGE_DATA_MAP.end()) {
		return it->second;
	}
	return "LOGO_DATA";  // Default to "LOGO_DATA" if selection is not found
}
