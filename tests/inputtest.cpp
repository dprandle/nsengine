#define INPUTTEST

#include <nsinputsystem.h>
#include <nsinputmap.h>
#include <iostream>
#include <nsengine.h>
#include <nsglfw.h>
#include <nsscene.h>
#include <iostream>
#include <nsplugin.h>

int main()
{
    glfw_setup(ivec2(400,400), false, "Build And Battle 1.0.0");
	
	nsuint i = nsengine.createContext();
    nsengine.start();

    NSPlugin * plg = nsengine.createPlugin("sillyplug");
    NSScene * scn = plg->create<NSScene>("mainscene");
	scn->setBackgroundColor(fvec3(0.7f, 0.7f, 1.0f));
	nsengine.setCurrentScene(scn, true);
	
    NSEntity * cam = plg->createCamera("scenecam", 60.0f, uivec2(400, 400), fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
	NSEntity * dirl = plg->createDirLight("dirlight", 0.8f, 0.2f);
	scn->setCamera(cam);
	scn->add(dirl, fvec3(20.0f, 20.0f, -20.0f));
    NSEntity * tile = plg->createTile("grasstile", nsengine.importdir() + "diffuseGrass.png", nsengine.importdir() + "normalGrass.png", fvec3(1.0, 1.0, 0.0), 16.0f, 0.5f, fvec3(1.0f), true);
    scn->addGridded(tile, ivec3(32, 32, 2), fvec3(0.0f,0.0f,0.0f));


	NSInputMap * im = plg->create<NSInputMap>("testinginput");
	im->createContext("Main");

	NSInputMap::Trigger camtogglemode(
        "CameraToggleMode",
        NSInputMap::Pressed
        );
    camtogglemode.addMouseModifier(NSInputMap::AnyButton);
    im->addKeyTrigger("Main", NSInputMap::Key_F, camtogglemode);

    NSInputMap::Trigger camtopview(
        "CameraTopView",
        NSInputMap::Pressed
        );
    camtopview.addMouseModifier(NSInputMap::AnyButton);
    im->addKeyTrigger("Main", NSInputMap::Key_KP8, camtopview);

    NSInputMap::Trigger cameraiso(
        "CameraIsoView",
        NSInputMap::Pressed
        );
    cameraiso.addMouseModifier(NSInputMap::AnyButton);
    im->addKeyTrigger("Main", NSInputMap::Key_KP5, cameraiso);

    NSInputMap::Trigger camfrontview(
        "CameraFrontView",
        NSInputMap::Pressed
        );
    camfrontview.addMouseModifier(NSInputMap::AnyButton);
    im->addKeyTrigger("Main", NSInputMap::Key_KP2, camfrontview);
	
	NSInputMap::Trigger camforward(
		"CameraForward",
		NSInputMap::Toggle
		);
	camforward.addMouseModifier(NSInputMap::AnyButton);
	im->addKeyTrigger("Main", NSInputMap::Key_W, camforward);

	
	NSInputMap::Trigger cambackward(
		"CameraBackward",
		NSInputMap::Toggle
		);
	cambackward.addMouseModifier(NSInputMap::AnyButton);
	im->addKeyTrigger("Main", NSInputMap::Key_S, cambackward);

	NSInputMap::Trigger camleft(
		"CameraLeft",
		NSInputMap::Toggle
		);
	camleft.addMouseModifier(NSInputMap::AnyButton);
	im->addKeyTrigger("Main", NSInputMap::Key_A, camleft);

	NSInputMap::Trigger camright(
		"CameraRight",
		NSInputMap::Toggle
		);
	camright.addMouseModifier(NSInputMap::AnyButton);
	im->addKeyTrigger("Main", NSInputMap::Key_D, camright);

	NSInputMap::Trigger camtilt(
		"CameraTiltPan",
		NSInputMap::Pressed,
		NSInputMap::MouseXDelta | NSInputMap::MouseYDelta);
    camtilt.addKeyModifier(NSInputMap::Key_Any);
    camtilt.addMouseModifier(NSInputMap::RightButton);
	im->addMouseTrigger("Main", NSInputMap::Movement, camtilt);

	NSInputMap::Trigger camzoom(
		"CameraZoom",
		NSInputMap::Pressed,
		NSInputMap::ScrollDelta);
	camzoom.addKeyModifier(NSInputMap::Key_Any);
    im->addMouseTrigger("Main", NSInputMap::Scrolling, camzoom);

	NSInputMap::Trigger cammove(
		"CameraMove",
		NSInputMap::Pressed,
		NSInputMap::MouseXDelta | NSInputMap::MouseYDelta
		);
	cammove.addKeyModifier(NSInputMap::Key_Any);
    cammove.addMouseModifier(NSInputMap::LeftButton);
    cammove.addMouseModifier(NSInputMap::RightButton);
    im->addMouseTrigger("Main", NSInputMap::Movement,cammove);

	NSInputMap::Trigger selectentity(
		"SelectEntity",
		NSInputMap::Pressed,
		NSInputMap::MouseXPos | NSInputMap::MouseYPos
		);
    //selectentity.addKeyModifier(NSInputMap::Key_Any);
    im->addMouseTrigger("Main", NSInputMap::LeftButton,selectentity);

	NSInputMap::Trigger shiftselect(
		"ShiftSelect",
		NSInputMap::Pressed,
		NSInputMap::MouseXPos | NSInputMap::MouseYPos
		);
	shiftselect.addKeyModifier(NSInputMap::Key_LShift);
    im->addMouseTrigger("Main", NSInputMap::Movement,shiftselect);

	NSInputMap::Trigger multiselect(
		"MultiSelect",
		NSInputMap::Pressed,
		NSInputMap::MouseXPos | NSInputMap::MouseYPos
		);
	multiselect.addKeyModifier(NSInputMap::Key_LCtrl);
    im->addMouseTrigger("Main", NSInputMap::LeftButton,multiselect);

    NSInputMap::Trigger selectmove(
        "MoveSelection",
        NSInputMap::Pressed,
        NSInputMap::MouseXDelta | NSInputMap::MouseYDelta
        );
    selectmove.addMouseModifier(NSInputMap::LeftButton);
    im->addMouseTrigger("Main", NSInputMap::Movement,selectmove);
	
	NSInputMap::Trigger selectmovexy(
		"MoveSelectionXY",
		NSInputMap::Pressed,
		NSInputMap::MouseXDelta | NSInputMap::MouseYDelta
		);
    selectmovexy.addKeyModifier(NSInputMap::Key_X);
    selectmovexy.addKeyModifier(NSInputMap::Key_Y);
	selectmovexy.addMouseModifier(NSInputMap::LeftButton);
    im->addMouseTrigger("Main", NSInputMap::Movement,selectmovexy);

	NSInputMap::Trigger selectmovezy(
		"MoveSelectionZY",
		NSInputMap::Pressed,
		NSInputMap::MouseXDelta | NSInputMap::MouseYDelta
		);
	selectmovezy.addKeyModifier(NSInputMap::Key_Z);
	selectmovezy.addKeyModifier(NSInputMap::Key_Y);
	selectmovezy.addMouseModifier(NSInputMap::LeftButton);
    im->addMouseTrigger("Main", NSInputMap::Movement,selectmovezy);

	NSInputMap::Trigger selectmovezx(
        "MoveSelectionZX",
		NSInputMap::Pressed,
		NSInputMap::MouseXDelta | NSInputMap::MouseYDelta
		);
	selectmovezx.addKeyModifier(NSInputMap::Key_Z);
	selectmovezx.addKeyModifier(NSInputMap::Key_X);
    selectmovezx.addMouseModifier(NSInputMap::LeftButton);
    im->addMouseTrigger("Main", NSInputMap::Movement,selectmovezx);


	NSInputMap::Trigger selectmovex(
		"MoveSelectionX",
		NSInputMap::Pressed,
		NSInputMap::MouseXDelta | NSInputMap::MouseYDelta
		);

	selectmovex.addMouseModifier(NSInputMap::LeftButton);
	selectmovex.addKeyModifier(NSInputMap::Key_X);
    im->addMouseTrigger("Main", NSInputMap::Movement,selectmovex);

	NSInputMap::Trigger selectmovey(
		"MoveSelectionY",
		NSInputMap::Pressed,
		NSInputMap::MouseXDelta | NSInputMap::MouseYDelta
		);
	selectmovey.addKeyModifier(NSInputMap::Key_Y);
	selectmovey.addMouseModifier(NSInputMap::LeftButton);
    im->addMouseTrigger("Main", NSInputMap::Movement,selectmovey);

	NSInputMap::Trigger selectmovez(
        "MoveSelectionZ",
		NSInputMap::Pressed,
		NSInputMap::MouseXDelta | NSInputMap::MouseYDelta
		);
	selectmovez.addKeyModifier(NSInputMap::Key_Z);
    selectmovez.addMouseModifier(NSInputMap::LeftButton);
    im->addMouseTrigger("Main", NSInputMap::Movement,selectmovez);

	NSInputMap::Trigger selectmovetoggle(
        "MoveSelectionToggle",
		NSInputMap::Toggle
		);
	selectmovetoggle.addKeyModifier(NSInputMap::Key_Any);
    im->addMouseTrigger("Main", NSInputMap::LeftButton,selectmovetoggle);

	nsengine.system<NSInputSystem>()->setInputMap(im->fullid());
	nsengine.system<NSInputSystem>()->pushContext("Main");
	
    while (glfw_window_open())
    {
        /* Render here */
		nsengine.update();
		glfw_update();
    }

	nsengine.shutdown();
    glfwTerminate();
    return 0;
}
