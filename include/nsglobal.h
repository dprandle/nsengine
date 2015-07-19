/* ---------------------------------Marlin Toolkit-----------------------------*
Authors : Daniel Randle, Alex Bourne
Date Created : Feb 25 2013

File:
	nsglobal.h

Description:
	This file contains the declaration for all the preproccessor globals along with other
	ones we might need
*-----------------------------------------------------------------------------*/
#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef _DEBUG
#define NSDEBUG
#define NSDEBUG_RT
#endif

#define NOTIFY_ENGINE_NAME_CHANGE

#define VERSION "1.00.00"
#define POS_ATT 0
#define TEX_ATT 1
#define NORM_ATT 2
#define TANG_ATT 3
#define BONEID_ATT 4
#define BONEWEIGHT_ATT 5
#define TRANS_ATT 6
#define REFID_ATT 10
#define X_GRID 0.86f
#define Y_GRID 1.49f
#define Z_GRID 0.45f
#define ROUND_FACTOR 0.5f
#define LIGHT_ATTENUATION_CUTOFF 0.01f

#define DIFFUSE_TEX_UNIT 0
#define NORMAL_TEX_UNIT 1
#define HEIGHT_TEX_UNIT 2
#define SPECULAR_TEX_UNIT 3
#define DISPLACEMENT_TEX_UNIT 4
#define AMBIENT_TEX_UNIT 5
#define EMISSIVE_TEX_UNIT 6
#define SHININESS_TEX_UNIT 7
#define OPACITY_TEX_UNIT 8
#define LIGHT_TEX_UNIT 9
#define REFLECTION_TEX_UNIT 10
#define G_DIFFUSE_TEX_UNIT 11
#define G_WORLD_POS_TEX_UNIT 12
#define G_NORMAL_TEX_UNIT 13
#define G_PICKING_TEX_UNIT 14
#define G_MATERIAL_TEX_UNIT 15
#define FINAL_TEX_UNIT 16
#define SKYBOX_TEX_UNIT 17
#define SHADOW_TEX_UNIT 18
#define RAND_TEX_UNIT 19
#define SCENE_MAX_PLAYERS 16

#define DEFAULT_RESOURCE_DIR "resources/"
#define DEFAULT_IMPORT_DIR "import/"
#define DEFAULT_CORE_DIR "core/"
#define LOCAL_ANIMATION_DIR_DEFAULT "anims/"
#define LOCAL_MESH_DIR_DEFAULT "meshes/"
#define LOCAL_SHADER_DIR_DEFAULT "shaders/"
#define LOCAL_TEXTURE_DIR_DEFAULT "textures/"
#define LOCAL_MATERIAL_DIR_DEFAULT "materials/"
#define LOCAL_ENTITY_DIR_DEFAULT "entities/"
#define LOCAL_INPUT_DIR_DEFAULT "input/"
#define LOCAL_SCENE_DIR_DEFAULT "maps/"
#define LOCAL_PLUGIN_DIR_DEFAULT "plugins/"
#define DEFAULT_DEBUG_LOG "enginedebug.log"
#define DEFAULT_GBUFFER_SHADER "gbufferdefault"
#define DEFAULT_XFBGBUFFER_SHADER "gbufferxtf"
#define DEFAULT_XFBGBUFFER_RENDER_SHADER "gbufferxtfrender"
#define DEFAULT_EARLYZ_SHADER "earlyz"
#define DEFAULT_XFBEARLYZ_SHADER "xfbearlyz"
#define DEFAULT_LIGHTSTENCIL_SHADER "lightstencil"
#define DEFAULT_SPOTLIGHT_SHADER "spotlight"
#define DEFAULT_DIRLIGHT_SHADER "directionlight"
#define DEFAULT_POINTLIGHT_SHADER "pointlight"
#define DEFAULT_SELECTION_SHADER "selectionsolid"
#define DEFAULT_POINTSHADOWMAP_SHADER "pointshadowmap"
#define DEFAULT_SPOTSHADOWMAP_SHADER "spotshadowmap"
#define DEFAULT_DIRSHADOWMAP_SHADER "dirshadowmap"
#define DEFAULT_XFBPOINTSHADOWMAP_SHADER "xfbpointshadowmap"
#define DEFAULT_XFBSPOTSHADOWMAP_SHADER "xfbspotshadowmap"
#define DEFAULT_XFBDIRSHADOWMAP_SHADER "xfbdirshadowmap"
#define DEFAULT_RENDER_PARTICLE_SHADER "renderparticle"
#define DEFAULT_PROCESS_PARTICLE_SHADER "xfbparticle"
#define DEFAULT_SKYBOX_SHADER "skybox"

#define DEFAULT_MATERIAL_NAME "default"
#define DEFAULT_MATERIAL_DIFFUSE "default"
#define DEFAULT_ENGINE_INPUT "enginedefault.nsi"

#define ENT_OBJECT_BRUSH "objectbrush"

#define MESH_FULL_TILE "fulltile"
#define MESH_HALF_TILE "halftile"
#define MESH_DIRLIGHT_BOUNDS "dirlightbounds"
#define MESH_SPOTLIGHT_BOUNDS "spotlightbounds"
#define MESH_POINTLIGHT_BOUNDS "pointlightbounds"
#define MESH_TERRAIN "terrain"
#define MESH_SKYDOME "skydome"

#define ENT_OCCUPY_TILE "OccupyTile"
#define ENT_SKYDOME "SkyDome"
#define ENT_SCENE_CAM "SceneViewCam"
#define ENT_DIR_LIGHT "DirLight"
#define MAT_SKYDOME "SkyBoxMat0"
#define TEX_SKYDOME "Skybox/Sun5Deg/Skydome"

// Default extensions
#define DEFAULT_TEX_EXTENSION ".png"
#define DEFAULT_MAT_EXTENSION ".mat"
#define DEFAULT_MESH_EXTENSION ".msh"
#define DEFAULT_ANIM_EXTENSION ".anm"
#define DEFAULT_SHADER_EXTENSION ".shr"
#define DEFAULT_SCENE_EXTENSION ".map"
#define DEFAULT_ENTITY_EXTENSION ".ent"
#define DEFAULT_PLUGIN_EXTENSION ".bbp"
#define CUBEMAP_TEX_EXTENSION ".cube"

#define DEFAULT_SPOTLIGHT_SHADOW_W 1024
#define DEFAULT_SPOTLIGHT_SHADOW_H 1024
#define DEFAULT_POINTLIGHT_SHADOW_W 1024
#define DEFAULT_POINTLIGHT_SHADOW_H 1024
#define DEFAULT_DIRLIGHT_SHADOW_W 2048
#define DEFAULT_DIRLIGHT_SHADOW_H 2048
#define DEFAULT_SHADOW_EPSILON 0.0001f
#define DEFAULT_SEL_R 0.0f
#define DEFAULT_SEL_G 1.0f
#define DEFAULT_SEL_B 1.0f
#define DEFAULT_SEL_A 0.5f
#define DEFAULT_SEL_MASK_A 0.2f
#define BONES_PER_VERTEX 4
#define DEFAULT_RENDER_W 1920
#define DEFAULT_RENDER_H 1080
#define DEFAULT_GRID_SIZE 64
#define QUADRANT_COUNT 8
#define TILE_GRID_RESIZE_PAD 16

#define DEFAULT_MAX_FORCE_KEYS 50
#define DEFAULT_MAX_RENDER_KEYS 50

#define BASE_TEX_UNIT GL_TEXTURE0
#define BASE_CUBEMAP_FACE GL_TEXTURE_CUBE_MAP_POSITIVE_X

#define DEFAULT_CAM_SENSITIVITY 80.0f
#define DEFAULT_CAM_SPEED 10.0f
#define DEFAULT_CAM_ZOOM_FACTOR 3.0f
#define DEFAULT_CAM_TOP_VIEW_HEIGHT 80.0f
#define DEFAULT_FOV_ANGLE 45.0f
#define DEFAULT_FB_RES_X 1920
#define DEFAULT_FB_RES_Y 1080
#define DEFAULT_Z_NEAR 1.0f
#define DEFAULT_Z_FAR 1000.0f
#define DEFAULT_CAM_VIEW_Z -15.0f

#define ENT_REF_ALLOWABLE_COMPS 3
#define ENT_ALLOWABLE_COMPS 4

#define MAGIC_STRING_SIZE 20
#define VERSION_SIZE 20
// magic strings
#define MAT_MAGIC_STRING "@#$MATERIAL@#$"
#define SHADER_MAGIC_STRING "@#$SHADER@#$"
#define INPUT_MAGIC_STRING "@#$INPUT@#$"
#define MESH_MAGIC_STRING "@#$MESH@#$"
#define ANIM_MAGIC_STRING "@#$ANIMATION@#$"
#define SCENE_MAGIC_STRING "@#$SCENE@#$"
#define ENT_MAGIC_STRING "@#$ENTITY@#$"
#define PLUG_MAGIC_STRING "@#PLUGIN@#$"

// versions
#define MATERIAL_VERSION "1.0.0"
#define SHADER_VERSION "1.0.0"
#define INPUT_VERSION "1.0.0"
#define MESH_VERSION "1.0.0"
#define ANIMATION_VERSION "1.0.0"
#define SCENE_VERSION "1.0.0"
#define ENTITY_VERSION "1.0.0"
#define PLUGIN_VERSION "1.0.0"

// text commands signaling file deserialization
#define SHADER_END "SHADER END"
#define FRAGMENT_BEGIN "FRAGMENT BEGIN"
#define GEOMETRY_BEGIN "GEOMETRY BEGIN"
#define VERTEX_BEGIN "VERTEX BEGIN"
#define FRAGMENT_END "FRAGMENT END"
#define GEOMETRY_END "GEOMETRY END"
#define VERTEX_END "VERTEX END"
#define INPUT_END "INPUT END"
#define PLUGIN_END "PLUGIN END"
#define NOTES_END "NOTES END"
#define PARENTS_END "PARENTS END"
#define PLUGIN_RES_SIGNAL "RES MANAGER"

// Material map type
#define TEXMAPTYPES 11

// Resource typenames
// Must add entry here for any custom resource types
#define ANIM_TYPESTRING "NSAnimSet"
#define ENTITY_TYPESTRING "NSEntity"
#define MATERIAL_TYPESTRING "NSMaterial"
#define MESH_TYPESTRING "NSMesh"
#define INPUTMAP_TYPESTRING "NSInputMap"
#define TEX1D_TYPESTRING "NSTex1D"
#define TEX1DARRAY_TYPESTRING "NSTex1DArray"
#define TEX2D_TYPESTRING "NSTex2D"
#define TEXRECTANGLE_TYPESTRING "NSTexRectangle"
#define TEX2DMULTISAMPLE_TYPESTRING "NSTex2DMultisample"
#define TEX2DMULTISAMPLEARRAY_TYPESTRING "NSTex2DMultisampleArray"
#define TEXBUFFER_TYPESTRING "NSTexBuffer"
#define TEX2DARRAY_TYPESTRING "NSTex2DArray"
#define TEX3D_TYPESTRING "NSTex3D"
#define TEXCUBEMAP_TYPESTRING "NSTexCubeMap"
#define TEXCUBEMAPARRAY_TYPESTRING "NSTexCubeMapArray"
#define SHADER_TYPESTRING "NSShader"
#define DIRLIGHTSHADER_TYPESTRING "NSDirLightShader"
#define POINTLIGHTSHADER_TYPESTRING "NSPointLightShader"
#define SPOTLIGHTSHADER_TYPESTRING "NSSpotLightShader"
#define MATERIALSHADER_TYPESTRING "NSMaterialShader"
#define PARTICLEPROCESSSHADER_TYPESTRING "NSParticleProcessShader"
#define PARTICLERENDERSHADER_TYPESTRING "NSParticleRenderShader"
#define DIR_SHADOWMAP_SHADER_TYPESTRING "NSDirShadowMapShader"
#define DIR_SHADOWMAPXFB_SHADER_TYPESTRING "NSDirShadowMapXFBShader"
#define POINT_SHADOWMAP_SHADER_TYPESTRING "NSPointShadowMapShader"
#define POINT_SHADOWMAPXFB_SHADER_TYPESTRING "NSPointShadowMapXFBShader"
#define SPOT_SHADOWMAP_SHADER_TYPESTRING "NSSpotShadowMapShader"
#define SPOT_SHADOWMAPXFB_SHADER_TYPESTRING "NSSpotShadowMapXFBShader"
#define EARLYZ_SHADER_TYPESTRING "NSEarlyZShader"
#define EARLYZXFB_SHADER_TYPESTRING "NSEarlyZXFBShader"
#define RENDERXFB_SHADER_TYPESTRING "NSRenderXFBShader"
#define XFB_SHADER_TYPESTRING "NSXFBShader"
#define LIGHTSTENCIL_SHADER_TYPESTRING "NSLightStencilShader"
#define SKYBOX_SHADER_TYPESTRING "NSSkyboxShader"
#define TRANSPARENCY_SHADER_TYPESTRING "NSTransparencyShader"
#define SELECTION_SHADER_TYPESTRING "NSSelectionShader"
#define SCENE_TYPESTRING "NSScene"
#define PLUGIN_TYPESTRING "NSPlugin"

#define ENGINE_PLUG "engplug"

#define ANIM_MANAGER_TYPESTRING "NSAnimManager"
#define ENTITY_MANAGER_TYPESTRING "NSEntityManager"
#define MAT_MANAGER_TYPESTRING "NSMatManager"
#define MESH_MANAGER_TYPESTRING "NSMeshManager"
#define TEX_MANAGER_TYPESTRING "NSTexManager"
#define SHADER_MANAGER_TYPESTRING "NSShaderManager"
#define SCENE_MANAGER_TYPESTRING "NSSceneManager"
#define PLUGIN_MANAGER_TYPESTRING "NSPluginManager"
#define INPUTMAP_MANAGER_TYPESTRING "NSInputMapManager"

// Component Typenames
#define ANIM_COMP_TYPESTRING "NSAnimComp"
#define CAM_COMP_TYPESTRING "NSCamComp"
#define ICON_COMP_TYPESTRING "NSIconComp"
#define INPUT_COMP_TYPESTRING "NSInputComp"
#define LIGHT_COMP_TYPESTRING "NSLightComp"
#define OCCUPY_COMP_TYPESTRING "NSOccupyComp"
#define RENDER_COMP_TYPESTRING "NSRenderComp"
#define SEL_COMP_TYPESTRING "NSSelComp"
#define TFORM_COMP_TYPESTRING "NSTFormComp"
#define TILEBRUSH_COMP_TYPESTRING "NSTileBrushComp"
#define TILE_COMP_TYPESTRING "NSTileComp"
#define PARTICLE_COMP_TYPESTRING "NSParticleComp"
#define TERRAIN_COMP_TYPESTRING "NSTerrainComp"

// System Typenames
#define ANIM_SYS_TYPESTRING "NSAnimationSystem"
#define CAM_SYS_TYPESTRING "NSCameraSystem"
#define BUILD_SYS_TYPESTRING "NSBuildSystem"
#define MOVE_SYS_TYPESTRING "NSMovementSystem"
#define RENDER_SYS_TYPESTRING "NSRenderSystem"
#define SEL_SYS_TYPESTRING "NSSelectionSystem"
#define UI_SYS_TYPESTRING "NSUISystem"
#define INP_SYS_TYPESTRING "NSInputSystem"
#define PARTICLE_SYS_TYPESTRING "NSParticleSystem"

// System update priority
#define ANIM_SYS_UPDATE_PR 70000
#define CAM_SYS_UPDATE_PR 40000
#define BUILD_SYS_UPDATE_PR 50000
#define MOVE_SYS_UPDATE_PR 60000
#define PARTICLE_SYS_UPDATE_PR 80000
#define RENDER_SYS_UPDATE_PR 90000
#define SEL_SYS_UPDATE_PR 30000
#define INP_SYS_UPDATE_PR 10000
#define UI_SYS_UPDATE_PR 20000

// System draw priority
#define RENDER_SYS_DRAW_PR 10000
#define PARTICLE_SYS_DRAW_PR 20000
#define SEL_SYS_DRAW_PR 30000
#define UI_SYS_DRAW_PR 40000
#define NO_DRAW_PR 0

#define FIXED_TIME_STEP 0.01f

#define DEFAULT_INPUT_CONTEXT "DefaultContext"
#define BUILD_MODE_CONTEXT "BuildMode"

// Input Actions
#define CAM_ZOOM "CamZoom"
#define CAM_MOVE "CamMove"
#define CAM_TURN "CamTurn"
#define CAM_FORWARD "CamForward"
#define CAM_BACKWARD "CamBackward"
#define CAM_LEFT "CamLeft"
#define CAM_RIGHT "CamRight"
#define SELECT "Select"
#define ROTATE_X "RotateX"
#define ROTATE_Y "RotateY"
#define ROTATE_Z "RotateZ"
#define MULTI_SELECT "MultiSelect"
#define PAINT_SELECT "PaintSelect"
#define DRAG_OBJECT_XY "DragObjectXY"
#define DRAG_OBJECT_XZ "DragObjectXZ"
#define DRAG_OBJECT_YZ "DragObjectYZ"
#define INSERT_OBJECT "InsertObject"
#define XZ_MOVE_END "XZMovementEnd"
#define YZ_MOVE_END "YZMovementEnd"
#define SHIFT_DONE "ShiftDone"

// Event names
#define RESOURCE_REMOVE "ResourceRemove"
#define RESOURCE_RENAME "ResourceRename"
#define RENDER_DEBUG "RenderDebug"

#define MAX_TF_BUFFER_SIZE 10048576
#define MAX_TF_BUFFER_COUNT 32 // about 100 mb

#define DEFAULT_MAX_PARTICLES 1000
#define DEFAULT_PART_LIFETIME 5000
#define DEFAULT_PART_EMISSION 180

#define nullchkn(ptr) if(ptr == NULL) return NULL;
#define nullchkb(ptr) if(ptr == NULL) return false;

#define nsengine NSEngine::inst()

#ifdef NSDEBUG
#define dprint(str) nsengine.debugPrint(str)
#else
#define dprint(str)
#endif

#include <myGL/glew.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <fstream>

// Basic type typedefs
typedef char nschar;
typedef char16_t nschar16;
typedef char32_t nschar32;
typedef wchar_t nswchar;

typedef short int nssint;
typedef int nsint;
typedef long int nslint;
typedef long long int nsllint;

typedef unsigned char nsuchar;
typedef unsigned short int nsusint;

typedef unsigned int nsuint;
typedef unsigned long int nsulint;
typedef unsigned long long int nsullint;

typedef float nsfloat;
typedef double nsdouble;
typedef long double nsldouble;

typedef bool nsbool;
typedef void nsvoid;

typedef std::string nsstring;
typedef std::stringstream nsstringstream;
typedef std::fstream nsfstream;

typedef std::vector<nschar> nschararray;
typedef std::vector<nschar16> nschar16array;
typedef std::vector<nschar32> nschar32array;
typedef std::vector<nswchar> nswchararray;
typedef std::vector<nssint> nssintarray;
typedef std::vector<nsint> nsintarray;
typedef std::vector<nslint> nslintarray;
typedef std::vector<nsllint> nsllintarray;
typedef std::vector<nsuchar> nsuchararray;
typedef std::vector<nsusint> nsusintarray;
typedef std::vector<nsuint> nsuintarray;
typedef std::vector<nsulint> nsulintarray;
typedef std::vector<nsullint> nsullintarray;
typedef std::vector<nsfloat> nsfloatarray;
typedef std::vector<nsdouble> nsdoublearray;
typedef std::vector<nsldouble> nsldoublearray;
typedef std::vector<nsbool> nsboolarray;
typedef std::vector<nsstring> nsstringarray;

typedef std::set<nschar> nscharset;
typedef std::set<nschar16> nschar16set;
typedef std::set<nschar32> nschar32set;
typedef std::set<nswchar> nswcharset;
typedef std::set<nssint> nssintset;
typedef std::set<nsint> nsintset;
typedef std::set<nslint> nslintset;
typedef std::set<nsllint> nsllintset;
typedef std::set<nsuchar> nsucharset;
typedef std::set<nsusint> nsusintset;
typedef std::set<nsuint> nsuintset;
typedef std::set<nsulint> nsulintset;
typedef std::set<nsullint> nsullintset;
typedef std::set<nsfloat> nsfloatset;
typedef std::set<nsdouble> nsdoubleset;
typedef std::set<nsldouble> nsldoubleset;
typedef std::set<nsbool> nsboolset;
typedef std::set<nsstring> nsstringset;

typedef std::unordered_set<nschar> nscharu_set;
typedef std::unordered_set<nschar16> nschar16u_set;
typedef std::unordered_set<nschar32> nschar32u_set;
typedef std::unordered_set<nswchar> nswcharu_set;
typedef std::unordered_set<nssint> nssintu_set;
typedef std::unordered_set<nsint> nsintu_set;
typedef std::unordered_set<nslint> nslintu_set;
typedef std::unordered_set<nsllint> nsllintu_set;
typedef std::unordered_set<nsuchar> nsucharu_set;
typedef std::unordered_set<nsusint> nsusintu_set;
typedef std::unordered_set<nsuint> nsuintu_set;
typedef std::unordered_set<nsulint> nsulintu_set;
typedef std::unordered_set<nsullint> nsullintu_set;
typedef std::unordered_set<nsfloat> nsfloatu_set;
typedef std::unordered_set<nsdouble> nsdoubleu_set;
typedef std::unordered_set<nsldouble> nsldoubleu_set;
typedef std::unordered_set<nsbool> nsboolu_set;
typedef std::unordered_set<nsstring> nsstringu_set;

typedef std::multiset<nschar> nscharmultiset;
typedef std::multiset<nschar16> nschar16multiset;
typedef std::multiset<nschar32> nschar32multiset;
typedef std::multiset<nswchar> nswcharmultiset;
typedef std::multiset<nssint> nssintmultiset;
typedef std::multiset<nsint> nsintmultiset;
typedef std::multiset<nslint> nslintmultiset;
typedef std::multiset<nsllint> nsllintmultiset;
typedef std::multiset<nsuchar> nsucharmultiset;
typedef std::multiset<nsusint> nsusintmultiset;
typedef std::multiset<nsuint> nsuintmultiset;
typedef std::multiset<nsulint> nsulintmultiset;
typedef std::multiset<nsullint> nsullintmultiset;
typedef std::multiset<nsfloat> nsfloatmultiset;
typedef std::multiset<nsdouble> nsdoublemultiset;
typedef std::multiset<nsldouble> nsldoublemultiset;
typedef std::multiset<nsbool> nsboolmultiset;
typedef std::multiset<nsstring> nsstringmultiset;

typedef std::map<nschar, nschar> nscharcharmap;
typedef std::map<nschar, nschar16> nscharchar16map;
typedef std::map<nschar, nschar32> nscharchar32map;
typedef std::map<nschar, nswchar> nscharwcharmap;
typedef std::map<nschar, nssint> nscharsintmap;
typedef std::map<nschar, nsint> nscharintmap;
typedef std::map<nschar, nslint> nscharlintmap;
typedef std::map<nschar, nsllint> nscharllintmap;
typedef std::map<nschar, nsuchar> nscharucharmap;
typedef std::map<nschar, nsusint> nscharusintmap;
typedef std::map<nschar, nsuint> nscharuintmap;
typedef std::map<nschar, nsulint> nscharulintmap;
typedef std::map<nschar, nsullint> nscharullintmap;
typedef std::map<nschar, nsfloat> nscharfloatmap;
typedef std::map<nschar, nsdouble> nschardoublemap;
typedef std::map<nschar, nsldouble> nscharldoublemap;
typedef std::map<nschar, nsbool> nscharboolmap;
typedef std::map<nschar, nsstring> nscharstringmap;
typedef std::map<nschar16, nschar> nschar16charmap;
typedef std::map<nschar16, nschar16> nschar16char16map;
typedef std::map<nschar16, nschar32> nschar16char32map;
typedef std::map<nschar16, nswchar> nschar16wcharmap;
typedef std::map<nschar16, nssint> nschar16sintmap;
typedef std::map<nschar16, nsint> nschar16intmap;
typedef std::map<nschar16, nslint> nschar16lintmap;
typedef std::map<nschar16, nsllint> nschar16llintmap;
typedef std::map<nschar16, nsuchar> nschar16ucharmap;
typedef std::map<nschar16, nsusint> nschar16usintmap;
typedef std::map<nschar16, nsuint> nschar16uintmap;
typedef std::map<nschar16, nsulint> nschar16ulintmap;
typedef std::map<nschar16, nsullint> nschar16ullintmap;
typedef std::map<nschar16, nsfloat> nschar16floatmap;
typedef std::map<nschar16, nsdouble> nschar16doublemap;
typedef std::map<nschar16, nsldouble> nschar16ldoublemap;
typedef std::map<nschar16, nsbool> nschar16boolmap;
typedef std::map<nschar16, nsstring> nschar16stringmap;
typedef std::map<nschar32, nschar> nschar32charmap;
typedef std::map<nschar32, nschar16> nschar32char16map;
typedef std::map<nschar32, nschar32> nschar32char32map;
typedef std::map<nschar32, nswchar> nschar32wcharmap;
typedef std::map<nschar32, nssint> nschar32sintmap;
typedef std::map<nschar32, nsint> nschar32intmap;
typedef std::map<nschar32, nslint> nschar32lintmap;
typedef std::map<nschar32, nsllint> nschar32llintmap;
typedef std::map<nschar32, nsuchar> nschar32ucharmap;
typedef std::map<nschar32, nsusint> nschar32usintmap;
typedef std::map<nschar32, nsuint> nschar32uintmap;
typedef std::map<nschar32, nsulint> nschar32ulintmap;
typedef std::map<nschar32, nsullint> nschar32ullintmap;
typedef std::map<nschar32, nsfloat> nschar32floatmap;
typedef std::map<nschar32, nsdouble> nschar32doublemap;
typedef std::map<nschar32, nsldouble> nschar32ldoublemap;
typedef std::map<nschar32, nsbool> nschar32boolmap;
typedef std::map<nschar32, nsstring> nschar32stringmap;
typedef std::map<nswchar, nschar> nswcharcharmap;
typedef std::map<nswchar, nschar16> nswcharchar16map;
typedef std::map<nswchar, nschar32> nswcharchar32map;
typedef std::map<nswchar, nswchar> nswcharwcharmap;
typedef std::map<nswchar, nssint> nswcharsintmap;
typedef std::map<nswchar, nsint> nswcharintmap;
typedef std::map<nswchar, nslint> nswcharlintmap;
typedef std::map<nswchar, nsllint> nswcharllintmap;
typedef std::map<nswchar, nsuchar> nswcharucharmap;
typedef std::map<nswchar, nsusint> nswcharusintmap;
typedef std::map<nswchar, nsuint> nswcharuintmap;
typedef std::map<nswchar, nsulint> nswcharulintmap;
typedef std::map<nswchar, nsullint> nswcharullintmap;
typedef std::map<nswchar, nsfloat> nswcharfloatmap;
typedef std::map<nswchar, nsdouble> nswchardoublemap;
typedef std::map<nswchar, nsldouble> nswcharldoublemap;
typedef std::map<nswchar, nsbool> nswcharboolmap;
typedef std::map<nswchar, nsstring> nswcharstringmap;
typedef std::map<nssint, nschar> nssintcharmap;
typedef std::map<nssint, nschar16> nssintchar16map;
typedef std::map<nssint, nschar32> nssintchar32map;
typedef std::map<nssint, nswchar> nssintwcharmap;
typedef std::map<nssint, nssint> nssintsintmap;
typedef std::map<nssint, nsint> nssintintmap;
typedef std::map<nssint, nslint> nssintlintmap;
typedef std::map<nssint, nsllint> nssintllintmap;
typedef std::map<nssint, nsuchar> nssintucharmap;
typedef std::map<nssint, nsusint> nssintusintmap;
typedef std::map<nssint, nsuint> nssintuintmap;
typedef std::map<nssint, nsulint> nssintulintmap;
typedef std::map<nssint, nsullint> nssintullintmap;
typedef std::map<nssint, nsfloat> nssintfloatmap;
typedef std::map<nssint, nsdouble> nssintdoublemap;
typedef std::map<nssint, nsldouble> nssintldoublemap;
typedef std::map<nssint, nsbool> nssintboolmap;
typedef std::map<nssint, nsstring> nssintstringmap;
typedef std::map<nsint, nschar> nsintcharmap;
typedef std::map<nsint, nschar16> nsintchar16map;
typedef std::map<nsint, nschar32> nsintchar32map;
typedef std::map<nsint, nswchar> nsintwcharmap;
typedef std::map<nsint, nssint> nsintsintmap;
typedef std::map<nsint, nsint> nsintintmap;
typedef std::map<nsint, nslint> nsintlintmap;
typedef std::map<nsint, nsllint> nsintllintmap;
typedef std::map<nsint, nsuchar> nsintucharmap;
typedef std::map<nsint, nsusint> nsintusintmap;
typedef std::map<nsint, nsuint> nsintuintmap;
typedef std::map<nsint, nsulint> nsintulintmap;
typedef std::map<nsint, nsullint> nsintullintmap;
typedef std::map<nsint, nsfloat> nsintfloatmap;
typedef std::map<nsint, nsdouble> nsintdoublemap;
typedef std::map<nsint, nsldouble> nsintldoublemap;
typedef std::map<nsint, nsbool> nsintboolmap;
typedef std::map<nsint, nsstring> nsintstringmap;
typedef std::map<nslint, nschar> nslintcharmap;
typedef std::map<nslint, nschar16> nslintchar16map;
typedef std::map<nslint, nschar32> nslintchar32map;
typedef std::map<nslint, nswchar> nslintwcharmap;
typedef std::map<nslint, nssint> nslintsintmap;
typedef std::map<nslint, nsint> nslintintmap;
typedef std::map<nslint, nslint> nslintlintmap;
typedef std::map<nslint, nsllint> nslintllintmap;
typedef std::map<nslint, nsuchar> nslintucharmap;
typedef std::map<nslint, nsusint> nslintusintmap;
typedef std::map<nslint, nsuint> nslintuintmap;
typedef std::map<nslint, nsulint> nslintulintmap;
typedef std::map<nslint, nsullint> nslintullintmap;
typedef std::map<nslint, nsfloat> nslintfloatmap;
typedef std::map<nslint, nsdouble> nslintdoublemap;
typedef std::map<nslint, nsldouble> nslintldoublemap;
typedef std::map<nslint, nsbool> nslintboolmap;
typedef std::map<nslint, nsstring> nslintstringmap;
typedef std::map<nsllint, nschar> nsllintcharmap;
typedef std::map<nsllint, nschar16> nsllintchar16map;
typedef std::map<nsllint, nschar32> nsllintchar32map;
typedef std::map<nsllint, nswchar> nsllintwcharmap;
typedef std::map<nsllint, nssint> nsllintsintmap;
typedef std::map<nsllint, nsint> nsllintintmap;
typedef std::map<nsllint, nslint> nsllintlintmap;
typedef std::map<nsllint, nsllint> nsllintllintmap;
typedef std::map<nsllint, nsuchar> nsllintucharmap;
typedef std::map<nsllint, nsusint> nsllintusintmap;
typedef std::map<nsllint, nsuint> nsllintuintmap;
typedef std::map<nsllint, nsulint> nsllintulintmap;
typedef std::map<nsllint, nsullint> nsllintullintmap;
typedef std::map<nsllint, nsfloat> nsllintfloatmap;
typedef std::map<nsllint, nsdouble> nsllintdoublemap;
typedef std::map<nsllint, nsldouble> nsllintldoublemap;
typedef std::map<nsllint, nsbool> nsllintboolmap;
typedef std::map<nsllint, nsstring> nsllintstringmap;
typedef std::map<nsuchar, nschar> nsucharcharmap;
typedef std::map<nsuchar, nschar16> nsucharchar16map;
typedef std::map<nsuchar, nschar32> nsucharchar32map;
typedef std::map<nsuchar, nswchar> nsucharwcharmap;
typedef std::map<nsuchar, nssint> nsucharsintmap;
typedef std::map<nsuchar, nsint> nsucharintmap;
typedef std::map<nsuchar, nslint> nsucharlintmap;
typedef std::map<nsuchar, nsllint> nsucharllintmap;
typedef std::map<nsuchar, nsuchar> nsucharucharmap;
typedef std::map<nsuchar, nsusint> nsucharusintmap;
typedef std::map<nsuchar, nsuint> nsucharuintmap;
typedef std::map<nsuchar, nsulint> nsucharulintmap;
typedef std::map<nsuchar, nsullint> nsucharullintmap;
typedef std::map<nsuchar, nsfloat> nsucharfloatmap;
typedef std::map<nsuchar, nsdouble> nsuchardoublemap;
typedef std::map<nsuchar, nsldouble> nsucharldoublemap;
typedef std::map<nsuchar, nsbool> nsucharboolmap;
typedef std::map<nsuchar, nsstring> nsucharstringmap;
typedef std::map<nsusint, nschar> nsusintcharmap;
typedef std::map<nsusint, nschar16> nsusintchar16map;
typedef std::map<nsusint, nschar32> nsusintchar32map;
typedef std::map<nsusint, nswchar> nsusintwcharmap;
typedef std::map<nsusint, nssint> nsusintsintmap;
typedef std::map<nsusint, nsint> nsusintintmap;
typedef std::map<nsusint, nslint> nsusintlintmap;
typedef std::map<nsusint, nsllint> nsusintllintmap;
typedef std::map<nsusint, nsuchar> nsusintucharmap;
typedef std::map<nsusint, nsusint> nsusintusintmap;
typedef std::map<nsusint, nsuint> nsusintuintmap;
typedef std::map<nsusint, nsulint> nsusintulintmap;
typedef std::map<nsusint, nsullint> nsusintullintmap;
typedef std::map<nsusint, nsfloat> nsusintfloatmap;
typedef std::map<nsusint, nsdouble> nsusintdoublemap;
typedef std::map<nsusint, nsldouble> nsusintldoublemap;
typedef std::map<nsusint, nsbool> nsusintboolmap;
typedef std::map<nsusint, nsstring> nsusintstringmap;
typedef std::map<nsuint, nschar> nsuintcharmap;
typedef std::map<nsuint, nschar16> nsuintchar16map;
typedef std::map<nsuint, nschar32> nsuintchar32map;
typedef std::map<nsuint, nswchar> nsuintwcharmap;
typedef std::map<nsuint, nssint> nsuintsintmap;
typedef std::map<nsuint, nsint> nsuintintmap;
typedef std::map<nsuint, nslint> nsuintlintmap;
typedef std::map<nsuint, nsllint> nsuintllintmap;
typedef std::map<nsuint, nsuchar> nsuintucharmap;
typedef std::map<nsuint, nsusint> nsuintusintmap;
typedef std::map<nsuint, nsuint> nsuintuintmap;
typedef std::map<nsuint, nsulint> nsuintulintmap;
typedef std::map<nsuint, nsullint> nsuintullintmap;
typedef std::map<nsuint, nsfloat> nsuintfloatmap;
typedef std::map<nsuint, nsdouble> nsuintdoublemap;
typedef std::map<nsuint, nsldouble> nsuintldoublemap;
typedef std::map<nsuint, nsbool> nsuintboolmap;
typedef std::map<nsuint, nsstring> nsuintstringmap;
typedef std::map<nsulint, nschar> nsulintcharmap;
typedef std::map<nsulint, nschar16> nsulintchar16map;
typedef std::map<nsulint, nschar32> nsulintchar32map;
typedef std::map<nsulint, nswchar> nsulintwcharmap;
typedef std::map<nsulint, nssint> nsulintsintmap;
typedef std::map<nsulint, nsint> nsulintintmap;
typedef std::map<nsulint, nslint> nsulintlintmap;
typedef std::map<nsulint, nsllint> nsulintllintmap;
typedef std::map<nsulint, nsuchar> nsulintucharmap;
typedef std::map<nsulint, nsusint> nsulintusintmap;
typedef std::map<nsulint, nsuint> nsulintuintmap;
typedef std::map<nsulint, nsulint> nsulintulintmap;
typedef std::map<nsulint, nsullint> nsulintullintmap;
typedef std::map<nsulint, nsfloat> nsulintfloatmap;
typedef std::map<nsulint, nsdouble> nsulintdoublemap;
typedef std::map<nsulint, nsldouble> nsulintldoublemap;
typedef std::map<nsulint, nsbool> nsulintboolmap;
typedef std::map<nsulint, nsstring> nsulintstringmap;
typedef std::map<nsullint, nschar> nsullintcharmap;
typedef std::map<nsullint, nschar16> nsullintchar16map;
typedef std::map<nsullint, nschar32> nsullintchar32map;
typedef std::map<nsullint, nswchar> nsullintwcharmap;
typedef std::map<nsullint, nssint> nsullintsintmap;
typedef std::map<nsullint, nsint> nsullintintmap;
typedef std::map<nsullint, nslint> nsullintlintmap;
typedef std::map<nsullint, nsllint> nsullintllintmap;
typedef std::map<nsullint, nsuchar> nsullintucharmap;
typedef std::map<nsullint, nsusint> nsullintusintmap;
typedef std::map<nsullint, nsuint> nsullintuintmap;
typedef std::map<nsullint, nsulint> nsullintulintmap;
typedef std::map<nsullint, nsullint> nsullintullintmap;
typedef std::map<nsullint, nsfloat> nsullintfloatmap;
typedef std::map<nsullint, nsdouble> nsullintdoublemap;
typedef std::map<nsullint, nsldouble> nsullintldoublemap;
typedef std::map<nsullint, nsbool> nsullintboolmap;
typedef std::map<nsullint, nsstring> nsullintstringmap;
typedef std::map<nsfloat, nschar> nsfloatcharmap;
typedef std::map<nsfloat, nschar16> nsfloatchar16map;
typedef std::map<nsfloat, nschar32> nsfloatchar32map;
typedef std::map<nsfloat, nswchar> nsfloatwcharmap;
typedef std::map<nsfloat, nssint> nsfloatsintmap;
typedef std::map<nsfloat, nsint> nsfloatintmap;
typedef std::map<nsfloat, nslint> nsfloatlintmap;
typedef std::map<nsfloat, nsllint> nsfloatllintmap;
typedef std::map<nsfloat, nsuchar> nsfloatucharmap;
typedef std::map<nsfloat, nsusint> nsfloatusintmap;
typedef std::map<nsfloat, nsuint> nsfloatuintmap;
typedef std::map<nsfloat, nsulint> nsfloatulintmap;
typedef std::map<nsfloat, nsullint> nsfloatullintmap;
typedef std::map<nsfloat, nsfloat> nsfloatfloatmap;
typedef std::map<nsfloat, nsdouble> nsfloatdoublemap;
typedef std::map<nsfloat, nsldouble> nsfloatldoublemap;
typedef std::map<nsfloat, nsbool> nsfloatboolmap;
typedef std::map<nsfloat, nsstring> nsfloatstringmap;
typedef std::map<nsdouble, nschar> nsdoublecharmap;
typedef std::map<nsdouble, nschar16> nsdoublechar16map;
typedef std::map<nsdouble, nschar32> nsdoublechar32map;
typedef std::map<nsdouble, nswchar> nsdoublewcharmap;
typedef std::map<nsdouble, nssint> nsdoublesintmap;
typedef std::map<nsdouble, nsint> nsdoubleintmap;
typedef std::map<nsdouble, nslint> nsdoublelintmap;
typedef std::map<nsdouble, nsllint> nsdoublellintmap;
typedef std::map<nsdouble, nsuchar> nsdoubleucharmap;
typedef std::map<nsdouble, nsusint> nsdoubleusintmap;
typedef std::map<nsdouble, nsuint> nsdoubleuintmap;
typedef std::map<nsdouble, nsulint> nsdoubleulintmap;
typedef std::map<nsdouble, nsullint> nsdoubleullintmap;
typedef std::map<nsdouble, nsfloat> nsdoublefloatmap;
typedef std::map<nsdouble, nsdouble> nsdoubledoublemap;
typedef std::map<nsdouble, nsldouble> nsdoubleldoublemap;
typedef std::map<nsdouble, nsbool> nsdoubleboolmap;
typedef std::map<nsdouble, nsstring> nsdoublestringmap;
typedef std::map<nsldouble, nschar> nsldoublecharmap;
typedef std::map<nsldouble, nschar16> nsldoublechar16map;
typedef std::map<nsldouble, nschar32> nsldoublechar32map;
typedef std::map<nsldouble, nswchar> nsldoublewcharmap;
typedef std::map<nsldouble, nssint> nsldoublesintmap;
typedef std::map<nsldouble, nsint> nsldoubleintmap;
typedef std::map<nsldouble, nslint> nsldoublelintmap;
typedef std::map<nsldouble, nsllint> nsldoublellintmap;
typedef std::map<nsldouble, nsuchar> nsldoubleucharmap;
typedef std::map<nsldouble, nsusint> nsldoubleusintmap;
typedef std::map<nsldouble, nsuint> nsldoubleuintmap;
typedef std::map<nsldouble, nsulint> nsldoubleulintmap;
typedef std::map<nsldouble, nsullint> nsldoubleullintmap;
typedef std::map<nsldouble, nsfloat> nsldoublefloatmap;
typedef std::map<nsldouble, nsdouble> nsldoubledoublemap;
typedef std::map<nsldouble, nsldouble> nsldoubleldoublemap;
typedef std::map<nsldouble, nsbool> nsldoubleboolmap;
typedef std::map<nsldouble, nsstring> nsldoublestringmap;
typedef std::map<nsbool, nschar> nsboolcharmap;
typedef std::map<nsbool, nschar16> nsboolchar16map;
typedef std::map<nsbool, nschar32> nsboolchar32map;
typedef std::map<nsbool, nswchar> nsboolwcharmap;
typedef std::map<nsbool, nssint> nsboolsintmap;
typedef std::map<nsbool, nsint> nsboolintmap;
typedef std::map<nsbool, nslint> nsboollintmap;
typedef std::map<nsbool, nsllint> nsboolllintmap;
typedef std::map<nsbool, nsuchar> nsboolucharmap;
typedef std::map<nsbool, nsusint> nsboolusintmap;
typedef std::map<nsbool, nsuint> nsbooluintmap;
typedef std::map<nsbool, nsulint> nsboolulintmap;
typedef std::map<nsbool, nsullint> nsboolullintmap;
typedef std::map<nsbool, nsfloat> nsboolfloatmap;
typedef std::map<nsbool, nsdouble> nsbooldoublemap;
typedef std::map<nsbool, nsldouble> nsboolldoublemap;
typedef std::map<nsbool, nsbool> nsboolboolmap;
typedef std::map<nsbool, nsstring> nsboolstringmap;
typedef std::map<nsstring, nschar> nsstringcharmap;
typedef std::map<nsstring, nschar16> nsstringchar16map;
typedef std::map<nsstring, nschar32> nsstringchar32map;
typedef std::map<nsstring, nswchar> nsstringwcharmap;
typedef std::map<nsstring, nssint> nsstringsintmap;
typedef std::map<nsstring, nsint> nsstringintmap;
typedef std::map<nsstring, nslint> nsstringlintmap;
typedef std::map<nsstring, nsllint> nsstringllintmap;
typedef std::map<nsstring, nsuchar> nsstringucharmap;
typedef std::map<nsstring, nsusint> nsstringusintmap;
typedef std::map<nsstring, nsuint> nsstringuintmap;
typedef std::map<nsstring, nsulint> nsstringulintmap;
typedef std::map<nsstring, nsullint> nsstringullintmap;
typedef std::map<nsstring, nsfloat> nsstringfloatmap;
typedef std::map<nsstring, nsdouble> nsstringdoublemap;
typedef std::map<nsstring, nsldouble> nsstringldoublemap;
typedef std::map<nsstring, nsbool> nsstringboolmap;
typedef std::map<nsstring, nsstring> nsstringstringmap;

typedef std::unordered_map<nschar, nschar> nscharcharu_map;
typedef std::unordered_map<nschar, nschar16> nscharchar16u_map;
typedef std::unordered_map<nschar, nschar32> nscharchar32u_map;
typedef std::unordered_map<nschar, nswchar> nscharwcharu_map;
typedef std::unordered_map<nschar, nssint> nscharsintu_map;
typedef std::unordered_map<nschar, nsint> nscharintu_map;
typedef std::unordered_map<nschar, nslint> nscharlintu_map;
typedef std::unordered_map<nschar, nsllint> nscharllintu_map;
typedef std::unordered_map<nschar, nsuchar> nscharucharu_map;
typedef std::unordered_map<nschar, nsusint> nscharusintu_map;
typedef std::unordered_map<nschar, nsuint> nscharuintu_map;
typedef std::unordered_map<nschar, nsulint> nscharulintu_map;
typedef std::unordered_map<nschar, nsullint> nscharullintu_map;
typedef std::unordered_map<nschar, nsfloat> nscharfloatu_map;
typedef std::unordered_map<nschar, nsdouble> nschardoubleu_map;
typedef std::unordered_map<nschar, nsldouble> nscharldoubleu_map;
typedef std::unordered_map<nschar, nsbool> nscharboolu_map;
typedef std::unordered_map<nschar, nsstring> nscharstringu_map;
typedef std::unordered_map<nschar16, nschar> nschar16charu_map;
typedef std::unordered_map<nschar16, nschar16> nschar16char16u_map;
typedef std::unordered_map<nschar16, nschar32> nschar16char32u_map;
typedef std::unordered_map<nschar16, nswchar> nschar16wcharu_map;
typedef std::unordered_map<nschar16, nssint> nschar16sintu_map;
typedef std::unordered_map<nschar16, nsint> nschar16intu_map;
typedef std::unordered_map<nschar16, nslint> nschar16lintu_map;
typedef std::unordered_map<nschar16, nsllint> nschar16llintu_map;
typedef std::unordered_map<nschar16, nsuchar> nschar16ucharu_map;
typedef std::unordered_map<nschar16, nsusint> nschar16usintu_map;
typedef std::unordered_map<nschar16, nsuint> nschar16uintu_map;
typedef std::unordered_map<nschar16, nsulint> nschar16ulintu_map;
typedef std::unordered_map<nschar16, nsullint> nschar16ullintu_map;
typedef std::unordered_map<nschar16, nsfloat> nschar16floatu_map;
typedef std::unordered_map<nschar16, nsdouble> nschar16doubleu_map;
typedef std::unordered_map<nschar16, nsldouble> nschar16ldoubleu_map;
typedef std::unordered_map<nschar16, nsbool> nschar16boolu_map;
typedef std::unordered_map<nschar16, nsstring> nschar16stringu_map;
typedef std::unordered_map<nschar32, nschar> nschar32charu_map;
typedef std::unordered_map<nschar32, nschar16> nschar32char16u_map;
typedef std::unordered_map<nschar32, nschar32> nschar32char32u_map;
typedef std::unordered_map<nschar32, nswchar> nschar32wcharu_map;
typedef std::unordered_map<nschar32, nssint> nschar32sintu_map;
typedef std::unordered_map<nschar32, nsint> nschar32intu_map;
typedef std::unordered_map<nschar32, nslint> nschar32lintu_map;
typedef std::unordered_map<nschar32, nsllint> nschar32llintu_map;
typedef std::unordered_map<nschar32, nsuchar> nschar32ucharu_map;
typedef std::unordered_map<nschar32, nsusint> nschar32usintu_map;
typedef std::unordered_map<nschar32, nsuint> nschar32uintu_map;
typedef std::unordered_map<nschar32, nsulint> nschar32ulintu_map;
typedef std::unordered_map<nschar32, nsullint> nschar32ullintu_map;
typedef std::unordered_map<nschar32, nsfloat> nschar32floatu_map;
typedef std::unordered_map<nschar32, nsdouble> nschar32doubleu_map;
typedef std::unordered_map<nschar32, nsldouble> nschar32ldoubleu_map;
typedef std::unordered_map<nschar32, nsbool> nschar32boolu_map;
typedef std::unordered_map<nschar32, nsstring> nschar32stringu_map;
typedef std::unordered_map<nswchar, nschar> nswcharcharu_map;
typedef std::unordered_map<nswchar, nschar16> nswcharchar16u_map;
typedef std::unordered_map<nswchar, nschar32> nswcharchar32u_map;
typedef std::unordered_map<nswchar, nswchar> nswcharwcharu_map;
typedef std::unordered_map<nswchar, nssint> nswcharsintu_map;
typedef std::unordered_map<nswchar, nsint> nswcharintu_map;
typedef std::unordered_map<nswchar, nslint> nswcharlintu_map;
typedef std::unordered_map<nswchar, nsllint> nswcharllintu_map;
typedef std::unordered_map<nswchar, nsuchar> nswcharucharu_map;
typedef std::unordered_map<nswchar, nsusint> nswcharusintu_map;
typedef std::unordered_map<nswchar, nsuint> nswcharuintu_map;
typedef std::unordered_map<nswchar, nsulint> nswcharulintu_map;
typedef std::unordered_map<nswchar, nsullint> nswcharullintu_map;
typedef std::unordered_map<nswchar, nsfloat> nswcharfloatu_map;
typedef std::unordered_map<nswchar, nsdouble> nswchardoubleu_map;
typedef std::unordered_map<nswchar, nsldouble> nswcharldoubleu_map;
typedef std::unordered_map<nswchar, nsbool> nswcharboolu_map;
typedef std::unordered_map<nswchar, nsstring> nswcharstringu_map;
typedef std::unordered_map<nssint, nschar> nssintcharu_map;
typedef std::unordered_map<nssint, nschar16> nssintchar16u_map;
typedef std::unordered_map<nssint, nschar32> nssintchar32u_map;
typedef std::unordered_map<nssint, nswchar> nssintwcharu_map;
typedef std::unordered_map<nssint, nssint> nssintsintu_map;
typedef std::unordered_map<nssint, nsint> nssintintu_map;
typedef std::unordered_map<nssint, nslint> nssintlintu_map;
typedef std::unordered_map<nssint, nsllint> nssintllintu_map;
typedef std::unordered_map<nssint, nsuchar> nssintucharu_map;
typedef std::unordered_map<nssint, nsusint> nssintusintu_map;
typedef std::unordered_map<nssint, nsuint> nssintuintu_map;
typedef std::unordered_map<nssint, nsulint> nssintulintu_map;
typedef std::unordered_map<nssint, nsullint> nssintullintu_map;
typedef std::unordered_map<nssint, nsfloat> nssintfloatu_map;
typedef std::unordered_map<nssint, nsdouble> nssintdoubleu_map;
typedef std::unordered_map<nssint, nsldouble> nssintldoubleu_map;
typedef std::unordered_map<nssint, nsbool> nssintboolu_map;
typedef std::unordered_map<nssint, nsstring> nssintstringu_map;
typedef std::unordered_map<nsint, nschar> nsintcharu_map;
typedef std::unordered_map<nsint, nschar16> nsintchar16u_map;
typedef std::unordered_map<nsint, nschar32> nsintchar32u_map;
typedef std::unordered_map<nsint, nswchar> nsintwcharu_map;
typedef std::unordered_map<nsint, nssint> nsintsintu_map;
typedef std::unordered_map<nsint, nsint> nsintintu_map;
typedef std::unordered_map<nsint, nslint> nsintlintu_map;
typedef std::unordered_map<nsint, nsllint> nsintllintu_map;
typedef std::unordered_map<nsint, nsuchar> nsintucharu_map;
typedef std::unordered_map<nsint, nsusint> nsintusintu_map;
typedef std::unordered_map<nsint, nsuint> nsintuintu_map;
typedef std::unordered_map<nsint, nsulint> nsintulintu_map;
typedef std::unordered_map<nsint, nsullint> nsintullintu_map;
typedef std::unordered_map<nsint, nsfloat> nsintfloatu_map;
typedef std::unordered_map<nsint, nsdouble> nsintdoubleu_map;
typedef std::unordered_map<nsint, nsldouble> nsintldoubleu_map;
typedef std::unordered_map<nsint, nsbool> nsintboolu_map;
typedef std::unordered_map<nsint, nsstring> nsintstringu_map;
typedef std::unordered_map<nslint, nschar> nslintcharu_map;
typedef std::unordered_map<nslint, nschar16> nslintchar16u_map;
typedef std::unordered_map<nslint, nschar32> nslintchar32u_map;
typedef std::unordered_map<nslint, nswchar> nslintwcharu_map;
typedef std::unordered_map<nslint, nssint> nslintsintu_map;
typedef std::unordered_map<nslint, nsint> nslintintu_map;
typedef std::unordered_map<nslint, nslint> nslintlintu_map;
typedef std::unordered_map<nslint, nsllint> nslintllintu_map;
typedef std::unordered_map<nslint, nsuchar> nslintucharu_map;
typedef std::unordered_map<nslint, nsusint> nslintusintu_map;
typedef std::unordered_map<nslint, nsuint> nslintuintu_map;
typedef std::unordered_map<nslint, nsulint> nslintulintu_map;
typedef std::unordered_map<nslint, nsullint> nslintullintu_map;
typedef std::unordered_map<nslint, nsfloat> nslintfloatu_map;
typedef std::unordered_map<nslint, nsdouble> nslintdoubleu_map;
typedef std::unordered_map<nslint, nsldouble> nslintldoubleu_map;
typedef std::unordered_map<nslint, nsbool> nslintboolu_map;
typedef std::unordered_map<nslint, nsstring> nslintstringu_map;
typedef std::unordered_map<nsllint, nschar> nsllintcharu_map;
typedef std::unordered_map<nsllint, nschar16> nsllintchar16u_map;
typedef std::unordered_map<nsllint, nschar32> nsllintchar32u_map;
typedef std::unordered_map<nsllint, nswchar> nsllintwcharu_map;
typedef std::unordered_map<nsllint, nssint> nsllintsintu_map;
typedef std::unordered_map<nsllint, nsint> nsllintintu_map;
typedef std::unordered_map<nsllint, nslint> nsllintlintu_map;
typedef std::unordered_map<nsllint, nsllint> nsllintllintu_map;
typedef std::unordered_map<nsllint, nsuchar> nsllintucharu_map;
typedef std::unordered_map<nsllint, nsusint> nsllintusintu_map;
typedef std::unordered_map<nsllint, nsuint> nsllintuintu_map;
typedef std::unordered_map<nsllint, nsulint> nsllintulintu_map;
typedef std::unordered_map<nsllint, nsullint> nsllintullintu_map;
typedef std::unordered_map<nsllint, nsfloat> nsllintfloatu_map;
typedef std::unordered_map<nsllint, nsdouble> nsllintdoubleu_map;
typedef std::unordered_map<nsllint, nsldouble> nsllintldoubleu_map;
typedef std::unordered_map<nsllint, nsbool> nsllintboolu_map;
typedef std::unordered_map<nsllint, nsstring> nsllintstringu_map;
typedef std::unordered_map<nsuchar, nschar> nsucharcharu_map;
typedef std::unordered_map<nsuchar, nschar16> nsucharchar16u_map;
typedef std::unordered_map<nsuchar, nschar32> nsucharchar32u_map;
typedef std::unordered_map<nsuchar, nswchar> nsucharwcharu_map;
typedef std::unordered_map<nsuchar, nssint> nsucharsintu_map;
typedef std::unordered_map<nsuchar, nsint> nsucharintu_map;
typedef std::unordered_map<nsuchar, nslint> nsucharlintu_map;
typedef std::unordered_map<nsuchar, nsllint> nsucharllintu_map;
typedef std::unordered_map<nsuchar, nsuchar> nsucharucharu_map;
typedef std::unordered_map<nsuchar, nsusint> nsucharusintu_map;
typedef std::unordered_map<nsuchar, nsuint> nsucharuintu_map;
typedef std::unordered_map<nsuchar, nsulint> nsucharulintu_map;
typedef std::unordered_map<nsuchar, nsullint> nsucharullintu_map;
typedef std::unordered_map<nsuchar, nsfloat> nsucharfloatu_map;
typedef std::unordered_map<nsuchar, nsdouble> nsuchardoubleu_map;
typedef std::unordered_map<nsuchar, nsldouble> nsucharldoubleu_map;
typedef std::unordered_map<nsuchar, nsbool> nsucharboolu_map;
typedef std::unordered_map<nsuchar, nsstring> nsucharstringu_map;
typedef std::unordered_map<nsusint, nschar> nsusintcharu_map;
typedef std::unordered_map<nsusint, nschar16> nsusintchar16u_map;
typedef std::unordered_map<nsusint, nschar32> nsusintchar32u_map;
typedef std::unordered_map<nsusint, nswchar> nsusintwcharu_map;
typedef std::unordered_map<nsusint, nssint> nsusintsintu_map;
typedef std::unordered_map<nsusint, nsint> nsusintintu_map;
typedef std::unordered_map<nsusint, nslint> nsusintlintu_map;
typedef std::unordered_map<nsusint, nsllint> nsusintllintu_map;
typedef std::unordered_map<nsusint, nsuchar> nsusintucharu_map;
typedef std::unordered_map<nsusint, nsusint> nsusintusintu_map;
typedef std::unordered_map<nsusint, nsuint> nsusintuintu_map;
typedef std::unordered_map<nsusint, nsulint> nsusintulintu_map;
typedef std::unordered_map<nsusint, nsullint> nsusintullintu_map;
typedef std::unordered_map<nsusint, nsfloat> nsusintfloatu_map;
typedef std::unordered_map<nsusint, nsdouble> nsusintdoubleu_map;
typedef std::unordered_map<nsusint, nsldouble> nsusintldoubleu_map;
typedef std::unordered_map<nsusint, nsbool> nsusintboolu_map;
typedef std::unordered_map<nsusint, nsstring> nsusintstringu_map;
typedef std::unordered_map<nsuint, nschar> nsuintcharu_map;
typedef std::unordered_map<nsuint, nschar16> nsuintchar16u_map;
typedef std::unordered_map<nsuint, nschar32> nsuintchar32u_map;
typedef std::unordered_map<nsuint, nswchar> nsuintwcharu_map;
typedef std::unordered_map<nsuint, nssint> nsuintsintu_map;
typedef std::unordered_map<nsuint, nsint> nsuintintu_map;
typedef std::unordered_map<nsuint, nslint> nsuintlintu_map;
typedef std::unordered_map<nsuint, nsllint> nsuintllintu_map;
typedef std::unordered_map<nsuint, nsuchar> nsuintucharu_map;
typedef std::unordered_map<nsuint, nsusint> nsuintusintu_map;
typedef std::unordered_map<nsuint, nsuint> nsuintuintu_map;
typedef std::unordered_map<nsuint, nsulint> nsuintulintu_map;
typedef std::unordered_map<nsuint, nsullint> nsuintullintu_map;
typedef std::unordered_map<nsuint, nsfloat> nsuintfloatu_map;
typedef std::unordered_map<nsuint, nsdouble> nsuintdoubleu_map;
typedef std::unordered_map<nsuint, nsldouble> nsuintldoubleu_map;
typedef std::unordered_map<nsuint, nsbool> nsuintboolu_map;
typedef std::unordered_map<nsuint, nsstring> nsuintstringu_map;
typedef std::unordered_map<nsulint, nschar> nsulintcharu_map;
typedef std::unordered_map<nsulint, nschar16> nsulintchar16u_map;
typedef std::unordered_map<nsulint, nschar32> nsulintchar32u_map;
typedef std::unordered_map<nsulint, nswchar> nsulintwcharu_map;
typedef std::unordered_map<nsulint, nssint> nsulintsintu_map;
typedef std::unordered_map<nsulint, nsint> nsulintintu_map;
typedef std::unordered_map<nsulint, nslint> nsulintlintu_map;
typedef std::unordered_map<nsulint, nsllint> nsulintllintu_map;
typedef std::unordered_map<nsulint, nsuchar> nsulintucharu_map;
typedef std::unordered_map<nsulint, nsusint> nsulintusintu_map;
typedef std::unordered_map<nsulint, nsuint> nsulintuintu_map;
typedef std::unordered_map<nsulint, nsulint> nsulintulintu_map;
typedef std::unordered_map<nsulint, nsullint> nsulintullintu_map;
typedef std::unordered_map<nsulint, nsfloat> nsulintfloatu_map;
typedef std::unordered_map<nsulint, nsdouble> nsulintdoubleu_map;
typedef std::unordered_map<nsulint, nsldouble> nsulintldoubleu_map;
typedef std::unordered_map<nsulint, nsbool> nsulintboolu_map;
typedef std::unordered_map<nsulint, nsstring> nsulintstringu_map;
typedef std::unordered_map<nsullint, nschar> nsullintcharu_map;
typedef std::unordered_map<nsullint, nschar16> nsullintchar16u_map;
typedef std::unordered_map<nsullint, nschar32> nsullintchar32u_map;
typedef std::unordered_map<nsullint, nswchar> nsullintwcharu_map;
typedef std::unordered_map<nsullint, nssint> nsullintsintu_map;
typedef std::unordered_map<nsullint, nsint> nsullintintu_map;
typedef std::unordered_map<nsullint, nslint> nsullintlintu_map;
typedef std::unordered_map<nsullint, nsllint> nsullintllintu_map;
typedef std::unordered_map<nsullint, nsuchar> nsullintucharu_map;
typedef std::unordered_map<nsullint, nsusint> nsullintusintu_map;
typedef std::unordered_map<nsullint, nsuint> nsullintuintu_map;
typedef std::unordered_map<nsullint, nsulint> nsullintulintu_map;
typedef std::unordered_map<nsullint, nsullint> nsullintullintu_map;
typedef std::unordered_map<nsullint, nsfloat> nsullintfloatu_map;
typedef std::unordered_map<nsullint, nsdouble> nsullintdoubleu_map;
typedef std::unordered_map<nsullint, nsldouble> nsullintldoubleu_map;
typedef std::unordered_map<nsullint, nsbool> nsullintboolu_map;
typedef std::unordered_map<nsullint, nsstring> nsullintstringu_map;
typedef std::unordered_map<nsfloat, nschar> nsfloatcharu_map;
typedef std::unordered_map<nsfloat, nschar16> nsfloatchar16u_map;
typedef std::unordered_map<nsfloat, nschar32> nsfloatchar32u_map;
typedef std::unordered_map<nsfloat, nswchar> nsfloatwcharu_map;
typedef std::unordered_map<nsfloat, nssint> nsfloatsintu_map;
typedef std::unordered_map<nsfloat, nsint> nsfloatintu_map;
typedef std::unordered_map<nsfloat, nslint> nsfloatlintu_map;
typedef std::unordered_map<nsfloat, nsllint> nsfloatllintu_map;
typedef std::unordered_map<nsfloat, nsuchar> nsfloatucharu_map;
typedef std::unordered_map<nsfloat, nsusint> nsfloatusintu_map;
typedef std::unordered_map<nsfloat, nsuint> nsfloatuintu_map;
typedef std::unordered_map<nsfloat, nsulint> nsfloatulintu_map;
typedef std::unordered_map<nsfloat, nsullint> nsfloatullintu_map;
typedef std::unordered_map<nsfloat, nsfloat> nsfloatfloatu_map;
typedef std::unordered_map<nsfloat, nsdouble> nsfloatdoubleu_map;
typedef std::unordered_map<nsfloat, nsldouble> nsfloatldoubleu_map;
typedef std::unordered_map<nsfloat, nsbool> nsfloatboolu_map;
typedef std::unordered_map<nsfloat, nsstring> nsfloatstringu_map;
typedef std::unordered_map<nsdouble, nschar> nsdoublecharu_map;
typedef std::unordered_map<nsdouble, nschar16> nsdoublechar16u_map;
typedef std::unordered_map<nsdouble, nschar32> nsdoublechar32u_map;
typedef std::unordered_map<nsdouble, nswchar> nsdoublewcharu_map;
typedef std::unordered_map<nsdouble, nssint> nsdoublesintu_map;
typedef std::unordered_map<nsdouble, nsint> nsdoubleintu_map;
typedef std::unordered_map<nsdouble, nslint> nsdoublelintu_map;
typedef std::unordered_map<nsdouble, nsllint> nsdoublellintu_map;
typedef std::unordered_map<nsdouble, nsuchar> nsdoubleucharu_map;
typedef std::unordered_map<nsdouble, nsusint> nsdoubleusintu_map;
typedef std::unordered_map<nsdouble, nsuint> nsdoubleuintu_map;
typedef std::unordered_map<nsdouble, nsulint> nsdoubleulintu_map;
typedef std::unordered_map<nsdouble, nsullint> nsdoubleullintu_map;
typedef std::unordered_map<nsdouble, nsfloat> nsdoublefloatu_map;
typedef std::unordered_map<nsdouble, nsdouble> nsdoubledoubleu_map;
typedef std::unordered_map<nsdouble, nsldouble> nsdoubleldoubleu_map;
typedef std::unordered_map<nsdouble, nsbool> nsdoubleboolu_map;
typedef std::unordered_map<nsdouble, nsstring> nsdoublestringu_map;
typedef std::unordered_map<nsldouble, nschar> nsldoublecharu_map;
typedef std::unordered_map<nsldouble, nschar16> nsldoublechar16u_map;
typedef std::unordered_map<nsldouble, nschar32> nsldoublechar32u_map;
typedef std::unordered_map<nsldouble, nswchar> nsldoublewcharu_map;
typedef std::unordered_map<nsldouble, nssint> nsldoublesintu_map;
typedef std::unordered_map<nsldouble, nsint> nsldoubleintu_map;
typedef std::unordered_map<nsldouble, nslint> nsldoublelintu_map;
typedef std::unordered_map<nsldouble, nsllint> nsldoublellintu_map;
typedef std::unordered_map<nsldouble, nsuchar> nsldoubleucharu_map;
typedef std::unordered_map<nsldouble, nsusint> nsldoubleusintu_map;
typedef std::unordered_map<nsldouble, nsuint> nsldoubleuintu_map;
typedef std::unordered_map<nsldouble, nsulint> nsldoubleulintu_map;
typedef std::unordered_map<nsldouble, nsullint> nsldoubleullintu_map;
typedef std::unordered_map<nsldouble, nsfloat> nsldoublefloatu_map;
typedef std::unordered_map<nsldouble, nsdouble> nsldoubledoubleu_map;
typedef std::unordered_map<nsldouble, nsldouble> nsldoubleldoubleu_map;
typedef std::unordered_map<nsldouble, nsbool> nsldoubleboolu_map;
typedef std::unordered_map<nsldouble, nsstring> nsldoublestringu_map;
typedef std::unordered_map<nsbool, nschar> nsboolcharu_map;
typedef std::unordered_map<nsbool, nschar16> nsboolchar16u_map;
typedef std::unordered_map<nsbool, nschar32> nsboolchar32u_map;
typedef std::unordered_map<nsbool, nswchar> nsboolwcharu_map;
typedef std::unordered_map<nsbool, nssint> nsboolsintu_map;
typedef std::unordered_map<nsbool, nsint> nsboolintu_map;
typedef std::unordered_map<nsbool, nslint> nsboollintu_map;
typedef std::unordered_map<nsbool, nsllint> nsboolllintu_map;
typedef std::unordered_map<nsbool, nsuchar> nsboolucharu_map;
typedef std::unordered_map<nsbool, nsusint> nsboolusintu_map;
typedef std::unordered_map<nsbool, nsuint> nsbooluintu_map;
typedef std::unordered_map<nsbool, nsulint> nsboolulintu_map;
typedef std::unordered_map<nsbool, nsullint> nsboolullintu_map;
typedef std::unordered_map<nsbool, nsfloat> nsboolfloatu_map;
typedef std::unordered_map<nsbool, nsdouble> nsbooldoubleu_map;
typedef std::unordered_map<nsbool, nsldouble> nsboolldoubleu_map;
typedef std::unordered_map<nsbool, nsbool> nsboolboolu_map;
typedef std::unordered_map<nsbool, nsstring> nsboolstringu_map;
typedef std::unordered_map<nsstring, nschar> nsstringcharu_map;
typedef std::unordered_map<nsstring, nschar16> nsstringchar16u_map;
typedef std::unordered_map<nsstring, nschar32> nsstringchar32u_map;
typedef std::unordered_map<nsstring, nswchar> nsstringwcharu_map;
typedef std::unordered_map<nsstring, nssint> nsstringsintu_map;
typedef std::unordered_map<nsstring, nsint> nsstringintu_map;
typedef std::unordered_map<nsstring, nslint> nsstringlintu_map;
typedef std::unordered_map<nsstring, nsllint> nsstringllintu_map;
typedef std::unordered_map<nsstring, nsuchar> nsstringucharu_map;
typedef std::unordered_map<nsstring, nsusint> nsstringusintu_map;
typedef std::unordered_map<nsstring, nsuint> nsstringuintu_map;
typedef std::unordered_map<nsstring, nsulint> nsstringulintu_map;
typedef std::unordered_map<nsstring, nsullint> nsstringullintu_map;
typedef std::unordered_map<nsstring, nsfloat> nsstringfloatu_map;
typedef std::unordered_map<nsstring, nsdouble> nsstringdoubleu_map;
typedef std::unordered_map<nsstring, nsldouble> nsstringldoubleu_map;
typedef std::unordered_map<nsstring, nsbool> nsstringboolu_map;
typedef std::unordered_map<nsstring, nsstring> nsstringstringu_map;

typedef std::multimap<nschar, nschar> nscharcharmultimap;
typedef std::multimap<nschar, nschar16> nscharchar16multimap;
typedef std::multimap<nschar, nschar32> nscharchar32multimap;
typedef std::multimap<nschar, nswchar> nscharwcharmultimap;
typedef std::multimap<nschar, nssint> nscharsintmultimap;
typedef std::multimap<nschar, nsint> nscharintmultimap;
typedef std::multimap<nschar, nslint> nscharlintmultimap;
typedef std::multimap<nschar, nsllint> nscharllintmultimap;
typedef std::multimap<nschar, nsuchar> nscharucharmultimap;
typedef std::multimap<nschar, nsusint> nscharusintmultimap;
typedef std::multimap<nschar, nsuint> nscharuintmultimap;
typedef std::multimap<nschar, nsulint> nscharulintmultimap;
typedef std::multimap<nschar, nsullint> nscharullintmultimap;
typedef std::multimap<nschar, nsfloat> nscharfloatmultimap;
typedef std::multimap<nschar, nsdouble> nschardoublemultimap;
typedef std::multimap<nschar, nsldouble> nscharldoublemultimap;
typedef std::multimap<nschar, nsbool> nscharboolmultimap;
typedef std::multimap<nschar, nsstring> nscharstringmultimap;
typedef std::multimap<nschar16, nschar> nschar16charmultimap;
typedef std::multimap<nschar16, nschar16> nschar16char16multimap;
typedef std::multimap<nschar16, nschar32> nschar16char32multimap;
typedef std::multimap<nschar16, nswchar> nschar16wcharmultimap;
typedef std::multimap<nschar16, nssint> nschar16sintmultimap;
typedef std::multimap<nschar16, nsint> nschar16intmultimap;
typedef std::multimap<nschar16, nslint> nschar16lintmultimap;
typedef std::multimap<nschar16, nsllint> nschar16llintmultimap;
typedef std::multimap<nschar16, nsuchar> nschar16ucharmultimap;
typedef std::multimap<nschar16, nsusint> nschar16usintmultimap;
typedef std::multimap<nschar16, nsuint> nschar16uintmultimap;
typedef std::multimap<nschar16, nsulint> nschar16ulintmultimap;
typedef std::multimap<nschar16, nsullint> nschar16ullintmultimap;
typedef std::multimap<nschar16, nsfloat> nschar16floatmultimap;
typedef std::multimap<nschar16, nsdouble> nschar16doublemultimap;
typedef std::multimap<nschar16, nsldouble> nschar16ldoublemultimap;
typedef std::multimap<nschar16, nsbool> nschar16boolmultimap;
typedef std::multimap<nschar16, nsstring> nschar16stringmultimap;
typedef std::multimap<nschar32, nschar> nschar32charmultimap;
typedef std::multimap<nschar32, nschar16> nschar32char16multimap;
typedef std::multimap<nschar32, nschar32> nschar32char32multimap;
typedef std::multimap<nschar32, nswchar> nschar32wcharmultimap;
typedef std::multimap<nschar32, nssint> nschar32sintmultimap;
typedef std::multimap<nschar32, nsint> nschar32intmultimap;
typedef std::multimap<nschar32, nslint> nschar32lintmultimap;
typedef std::multimap<nschar32, nsllint> nschar32llintmultimap;
typedef std::multimap<nschar32, nsuchar> nschar32ucharmultimap;
typedef std::multimap<nschar32, nsusint> nschar32usintmultimap;
typedef std::multimap<nschar32, nsuint> nschar32uintmultimap;
typedef std::multimap<nschar32, nsulint> nschar32ulintmultimap;
typedef std::multimap<nschar32, nsullint> nschar32ullintmultimap;
typedef std::multimap<nschar32, nsfloat> nschar32floatmultimap;
typedef std::multimap<nschar32, nsdouble> nschar32doublemultimap;
typedef std::multimap<nschar32, nsldouble> nschar32ldoublemultimap;
typedef std::multimap<nschar32, nsbool> nschar32boolmultimap;
typedef std::multimap<nschar32, nsstring> nschar32stringmultimap;
typedef std::multimap<nswchar, nschar> nswcharcharmultimap;
typedef std::multimap<nswchar, nschar16> nswcharchar16multimap;
typedef std::multimap<nswchar, nschar32> nswcharchar32multimap;
typedef std::multimap<nswchar, nswchar> nswcharwcharmultimap;
typedef std::multimap<nswchar, nssint> nswcharsintmultimap;
typedef std::multimap<nswchar, nsint> nswcharintmultimap;
typedef std::multimap<nswchar, nslint> nswcharlintmultimap;
typedef std::multimap<nswchar, nsllint> nswcharllintmultimap;
typedef std::multimap<nswchar, nsuchar> nswcharucharmultimap;
typedef std::multimap<nswchar, nsusint> nswcharusintmultimap;
typedef std::multimap<nswchar, nsuint> nswcharuintmultimap;
typedef std::multimap<nswchar, nsulint> nswcharulintmultimap;
typedef std::multimap<nswchar, nsullint> nswcharullintmultimap;
typedef std::multimap<nswchar, nsfloat> nswcharfloatmultimap;
typedef std::multimap<nswchar, nsdouble> nswchardoublemultimap;
typedef std::multimap<nswchar, nsldouble> nswcharldoublemultimap;
typedef std::multimap<nswchar, nsbool> nswcharboolmultimap;
typedef std::multimap<nswchar, nsstring> nswcharstringmultimap;
typedef std::multimap<nssint, nschar> nssintcharmultimap;
typedef std::multimap<nssint, nschar16> nssintchar16multimap;
typedef std::multimap<nssint, nschar32> nssintchar32multimap;
typedef std::multimap<nssint, nswchar> nssintwcharmultimap;
typedef std::multimap<nssint, nssint> nssintsintmultimap;
typedef std::multimap<nssint, nsint> nssintintmultimap;
typedef std::multimap<nssint, nslint> nssintlintmultimap;
typedef std::multimap<nssint, nsllint> nssintllintmultimap;
typedef std::multimap<nssint, nsuchar> nssintucharmultimap;
typedef std::multimap<nssint, nsusint> nssintusintmultimap;
typedef std::multimap<nssint, nsuint> nssintuintmultimap;
typedef std::multimap<nssint, nsulint> nssintulintmultimap;
typedef std::multimap<nssint, nsullint> nssintullintmultimap;
typedef std::multimap<nssint, nsfloat> nssintfloatmultimap;
typedef std::multimap<nssint, nsdouble> nssintdoublemultimap;
typedef std::multimap<nssint, nsldouble> nssintldoublemultimap;
typedef std::multimap<nssint, nsbool> nssintboolmultimap;
typedef std::multimap<nssint, nsstring> nssintstringmultimap;
typedef std::multimap<nsint, nschar> nsintcharmultimap;
typedef std::multimap<nsint, nschar16> nsintchar16multimap;
typedef std::multimap<nsint, nschar32> nsintchar32multimap;
typedef std::multimap<nsint, nswchar> nsintwcharmultimap;
typedef std::multimap<nsint, nssint> nsintsintmultimap;
typedef std::multimap<nsint, nsint> nsintintmultimap;
typedef std::multimap<nsint, nslint> nsintlintmultimap;
typedef std::multimap<nsint, nsllint> nsintllintmultimap;
typedef std::multimap<nsint, nsuchar> nsintucharmultimap;
typedef std::multimap<nsint, nsusint> nsintusintmultimap;
typedef std::multimap<nsint, nsuint> nsintuintmultimap;
typedef std::multimap<nsint, nsulint> nsintulintmultimap;
typedef std::multimap<nsint, nsullint> nsintullintmultimap;
typedef std::multimap<nsint, nsfloat> nsintfloatmultimap;
typedef std::multimap<nsint, nsdouble> nsintdoublemultimap;
typedef std::multimap<nsint, nsldouble> nsintldoublemultimap;
typedef std::multimap<nsint, nsbool> nsintboolmultimap;
typedef std::multimap<nsint, nsstring> nsintstringmultimap;
typedef std::multimap<nslint, nschar> nslintcharmultimap;
typedef std::multimap<nslint, nschar16> nslintchar16multimap;
typedef std::multimap<nslint, nschar32> nslintchar32multimap;
typedef std::multimap<nslint, nswchar> nslintwcharmultimap;
typedef std::multimap<nslint, nssint> nslintsintmultimap;
typedef std::multimap<nslint, nsint> nslintintmultimap;
typedef std::multimap<nslint, nslint> nslintlintmultimap;
typedef std::multimap<nslint, nsllint> nslintllintmultimap;
typedef std::multimap<nslint, nsuchar> nslintucharmultimap;
typedef std::multimap<nslint, nsusint> nslintusintmultimap;
typedef std::multimap<nslint, nsuint> nslintuintmultimap;
typedef std::multimap<nslint, nsulint> nslintulintmultimap;
typedef std::multimap<nslint, nsullint> nslintullintmultimap;
typedef std::multimap<nslint, nsfloat> nslintfloatmultimap;
typedef std::multimap<nslint, nsdouble> nslintdoublemultimap;
typedef std::multimap<nslint, nsldouble> nslintldoublemultimap;
typedef std::multimap<nslint, nsbool> nslintboolmultimap;
typedef std::multimap<nslint, nsstring> nslintstringmultimap;
typedef std::multimap<nsllint, nschar> nsllintcharmultimap;
typedef std::multimap<nsllint, nschar16> nsllintchar16multimap;
typedef std::multimap<nsllint, nschar32> nsllintchar32multimap;
typedef std::multimap<nsllint, nswchar> nsllintwcharmultimap;
typedef std::multimap<nsllint, nssint> nsllintsintmultimap;
typedef std::multimap<nsllint, nsint> nsllintintmultimap;
typedef std::multimap<nsllint, nslint> nsllintlintmultimap;
typedef std::multimap<nsllint, nsllint> nsllintllintmultimap;
typedef std::multimap<nsllint, nsuchar> nsllintucharmultimap;
typedef std::multimap<nsllint, nsusint> nsllintusintmultimap;
typedef std::multimap<nsllint, nsuint> nsllintuintmultimap;
typedef std::multimap<nsllint, nsulint> nsllintulintmultimap;
typedef std::multimap<nsllint, nsullint> nsllintullintmultimap;
typedef std::multimap<nsllint, nsfloat> nsllintfloatmultimap;
typedef std::multimap<nsllint, nsdouble> nsllintdoublemultimap;
typedef std::multimap<nsllint, nsldouble> nsllintldoublemultimap;
typedef std::multimap<nsllint, nsbool> nsllintboolmultimap;
typedef std::multimap<nsllint, nsstring> nsllintstringmultimap;
typedef std::multimap<nsuchar, nschar> nsucharcharmultimap;
typedef std::multimap<nsuchar, nschar16> nsucharchar16multimap;
typedef std::multimap<nsuchar, nschar32> nsucharchar32multimap;
typedef std::multimap<nsuchar, nswchar> nsucharwcharmultimap;
typedef std::multimap<nsuchar, nssint> nsucharsintmultimap;
typedef std::multimap<nsuchar, nsint> nsucharintmultimap;
typedef std::multimap<nsuchar, nslint> nsucharlintmultimap;
typedef std::multimap<nsuchar, nsllint> nsucharllintmultimap;
typedef std::multimap<nsuchar, nsuchar> nsucharucharmultimap;
typedef std::multimap<nsuchar, nsusint> nsucharusintmultimap;
typedef std::multimap<nsuchar, nsuint> nsucharuintmultimap;
typedef std::multimap<nsuchar, nsulint> nsucharulintmultimap;
typedef std::multimap<nsuchar, nsullint> nsucharullintmultimap;
typedef std::multimap<nsuchar, nsfloat> nsucharfloatmultimap;
typedef std::multimap<nsuchar, nsdouble> nsuchardoublemultimap;
typedef std::multimap<nsuchar, nsldouble> nsucharldoublemultimap;
typedef std::multimap<nsuchar, nsbool> nsucharboolmultimap;
typedef std::multimap<nsuchar, nsstring> nsucharstringmultimap;
typedef std::multimap<nsusint, nschar> nsusintcharmultimap;
typedef std::multimap<nsusint, nschar16> nsusintchar16multimap;
typedef std::multimap<nsusint, nschar32> nsusintchar32multimap;
typedef std::multimap<nsusint, nswchar> nsusintwcharmultimap;
typedef std::multimap<nsusint, nssint> nsusintsintmultimap;
typedef std::multimap<nsusint, nsint> nsusintintmultimap;
typedef std::multimap<nsusint, nslint> nsusintlintmultimap;
typedef std::multimap<nsusint, nsllint> nsusintllintmultimap;
typedef std::multimap<nsusint, nsuchar> nsusintucharmultimap;
typedef std::multimap<nsusint, nsusint> nsusintusintmultimap;
typedef std::multimap<nsusint, nsuint> nsusintuintmultimap;
typedef std::multimap<nsusint, nsulint> nsusintulintmultimap;
typedef std::multimap<nsusint, nsullint> nsusintullintmultimap;
typedef std::multimap<nsusint, nsfloat> nsusintfloatmultimap;
typedef std::multimap<nsusint, nsdouble> nsusintdoublemultimap;
typedef std::multimap<nsusint, nsldouble> nsusintldoublemultimap;
typedef std::multimap<nsusint, nsbool> nsusintboolmultimap;
typedef std::multimap<nsusint, nsstring> nsusintstringmultimap;
typedef std::multimap<nsuint, nschar> nsuintcharmultimap;
typedef std::multimap<nsuint, nschar16> nsuintchar16multimap;
typedef std::multimap<nsuint, nschar32> nsuintchar32multimap;
typedef std::multimap<nsuint, nswchar> nsuintwcharmultimap;
typedef std::multimap<nsuint, nssint> nsuintsintmultimap;
typedef std::multimap<nsuint, nsint> nsuintintmultimap;
typedef std::multimap<nsuint, nslint> nsuintlintmultimap;
typedef std::multimap<nsuint, nsllint> nsuintllintmultimap;
typedef std::multimap<nsuint, nsuchar> nsuintucharmultimap;
typedef std::multimap<nsuint, nsusint> nsuintusintmultimap;
typedef std::multimap<nsuint, nsuint> nsuintuintmultimap;
typedef std::multimap<nsuint, nsulint> nsuintulintmultimap;
typedef std::multimap<nsuint, nsullint> nsuintullintmultimap;
typedef std::multimap<nsuint, nsfloat> nsuintfloatmultimap;
typedef std::multimap<nsuint, nsdouble> nsuintdoublemultimap;
typedef std::multimap<nsuint, nsldouble> nsuintldoublemultimap;
typedef std::multimap<nsuint, nsbool> nsuintboolmultimap;
typedef std::multimap<nsuint, nsstring> nsuintstringmultimap;
typedef std::multimap<nsulint, nschar> nsulintcharmultimap;
typedef std::multimap<nsulint, nschar16> nsulintchar16multimap;
typedef std::multimap<nsulint, nschar32> nsulintchar32multimap;
typedef std::multimap<nsulint, nswchar> nsulintwcharmultimap;
typedef std::multimap<nsulint, nssint> nsulintsintmultimap;
typedef std::multimap<nsulint, nsint> nsulintintmultimap;
typedef std::multimap<nsulint, nslint> nsulintlintmultimap;
typedef std::multimap<nsulint, nsllint> nsulintllintmultimap;
typedef std::multimap<nsulint, nsuchar> nsulintucharmultimap;
typedef std::multimap<nsulint, nsusint> nsulintusintmultimap;
typedef std::multimap<nsulint, nsuint> nsulintuintmultimap;
typedef std::multimap<nsulint, nsulint> nsulintulintmultimap;
typedef std::multimap<nsulint, nsullint> nsulintullintmultimap;
typedef std::multimap<nsulint, nsfloat> nsulintfloatmultimap;
typedef std::multimap<nsulint, nsdouble> nsulintdoublemultimap;
typedef std::multimap<nsulint, nsldouble> nsulintldoublemultimap;
typedef std::multimap<nsulint, nsbool> nsulintboolmultimap;
typedef std::multimap<nsulint, nsstring> nsulintstringmultimap;
typedef std::multimap<nsullint, nschar> nsullintcharmultimap;
typedef std::multimap<nsullint, nschar16> nsullintchar16multimap;
typedef std::multimap<nsullint, nschar32> nsullintchar32multimap;
typedef std::multimap<nsullint, nswchar> nsullintwcharmultimap;
typedef std::multimap<nsullint, nssint> nsullintsintmultimap;
typedef std::multimap<nsullint, nsint> nsullintintmultimap;
typedef std::multimap<nsullint, nslint> nsullintlintmultimap;
typedef std::multimap<nsullint, nsllint> nsullintllintmultimap;
typedef std::multimap<nsullint, nsuchar> nsullintucharmultimap;
typedef std::multimap<nsullint, nsusint> nsullintusintmultimap;
typedef std::multimap<nsullint, nsuint> nsullintuintmultimap;
typedef std::multimap<nsullint, nsulint> nsullintulintmultimap;
typedef std::multimap<nsullint, nsullint> nsullintullintmultimap;
typedef std::multimap<nsullint, nsfloat> nsullintfloatmultimap;
typedef std::multimap<nsullint, nsdouble> nsullintdoublemultimap;
typedef std::multimap<nsullint, nsldouble> nsullintldoublemultimap;
typedef std::multimap<nsullint, nsbool> nsullintboolmultimap;
typedef std::multimap<nsullint, nsstring> nsullintstringmultimap;
typedef std::multimap<nsfloat, nschar> nsfloatcharmultimap;
typedef std::multimap<nsfloat, nschar16> nsfloatchar16multimap;
typedef std::multimap<nsfloat, nschar32> nsfloatchar32multimap;
typedef std::multimap<nsfloat, nswchar> nsfloatwcharmultimap;
typedef std::multimap<nsfloat, nssint> nsfloatsintmultimap;
typedef std::multimap<nsfloat, nsint> nsfloatintmultimap;
typedef std::multimap<nsfloat, nslint> nsfloatlintmultimap;
typedef std::multimap<nsfloat, nsllint> nsfloatllintmultimap;
typedef std::multimap<nsfloat, nsuchar> nsfloatucharmultimap;
typedef std::multimap<nsfloat, nsusint> nsfloatusintmultimap;
typedef std::multimap<nsfloat, nsuint> nsfloatuintmultimap;
typedef std::multimap<nsfloat, nsulint> nsfloatulintmultimap;
typedef std::multimap<nsfloat, nsullint> nsfloatullintmultimap;
typedef std::multimap<nsfloat, nsfloat> nsfloatfloatmultimap;
typedef std::multimap<nsfloat, nsdouble> nsfloatdoublemultimap;
typedef std::multimap<nsfloat, nsldouble> nsfloatldoublemultimap;
typedef std::multimap<nsfloat, nsbool> nsfloatboolmultimap;
typedef std::multimap<nsfloat, nsstring> nsfloatstringmultimap;
typedef std::multimap<nsdouble, nschar> nsdoublecharmultimap;
typedef std::multimap<nsdouble, nschar16> nsdoublechar16multimap;
typedef std::multimap<nsdouble, nschar32> nsdoublechar32multimap;
typedef std::multimap<nsdouble, nswchar> nsdoublewcharmultimap;
typedef std::multimap<nsdouble, nssint> nsdoublesintmultimap;
typedef std::multimap<nsdouble, nsint> nsdoubleintmultimap;
typedef std::multimap<nsdouble, nslint> nsdoublelintmultimap;
typedef std::multimap<nsdouble, nsllint> nsdoublellintmultimap;
typedef std::multimap<nsdouble, nsuchar> nsdoubleucharmultimap;
typedef std::multimap<nsdouble, nsusint> nsdoubleusintmultimap;
typedef std::multimap<nsdouble, nsuint> nsdoubleuintmultimap;
typedef std::multimap<nsdouble, nsulint> nsdoubleulintmultimap;
typedef std::multimap<nsdouble, nsullint> nsdoubleullintmultimap;
typedef std::multimap<nsdouble, nsfloat> nsdoublefloatmultimap;
typedef std::multimap<nsdouble, nsdouble> nsdoubledoublemultimap;
typedef std::multimap<nsdouble, nsldouble> nsdoubleldoublemultimap;
typedef std::multimap<nsdouble, nsbool> nsdoubleboolmultimap;
typedef std::multimap<nsdouble, nsstring> nsdoublestringmultimap;
typedef std::multimap<nsldouble, nschar> nsldoublecharmultimap;
typedef std::multimap<nsldouble, nschar16> nsldoublechar16multimap;
typedef std::multimap<nsldouble, nschar32> nsldoublechar32multimap;
typedef std::multimap<nsldouble, nswchar> nsldoublewcharmultimap;
typedef std::multimap<nsldouble, nssint> nsldoublesintmultimap;
typedef std::multimap<nsldouble, nsint> nsldoubleintmultimap;
typedef std::multimap<nsldouble, nslint> nsldoublelintmultimap;
typedef std::multimap<nsldouble, nsllint> nsldoublellintmultimap;
typedef std::multimap<nsldouble, nsuchar> nsldoubleucharmultimap;
typedef std::multimap<nsldouble, nsusint> nsldoubleusintmultimap;
typedef std::multimap<nsldouble, nsuint> nsldoubleuintmultimap;
typedef std::multimap<nsldouble, nsulint> nsldoubleulintmultimap;
typedef std::multimap<nsldouble, nsullint> nsldoubleullintmultimap;
typedef std::multimap<nsldouble, nsfloat> nsldoublefloatmultimap;
typedef std::multimap<nsldouble, nsdouble> nsldoubledoublemultimap;
typedef std::multimap<nsldouble, nsldouble> nsldoubleldoublemultimap;
typedef std::multimap<nsldouble, nsbool> nsldoubleboolmultimap;
typedef std::multimap<nsldouble, nsstring> nsldoublestringmultimap;
typedef std::multimap<nsbool, nschar> nsboolcharmultimap;
typedef std::multimap<nsbool, nschar16> nsboolchar16multimap;
typedef std::multimap<nsbool, nschar32> nsboolchar32multimap;
typedef std::multimap<nsbool, nswchar> nsboolwcharmultimap;
typedef std::multimap<nsbool, nssint> nsboolsintmultimap;
typedef std::multimap<nsbool, nsint> nsboolintmultimap;
typedef std::multimap<nsbool, nslint> nsboollintmultimap;
typedef std::multimap<nsbool, nsllint> nsboolllintmultimap;
typedef std::multimap<nsbool, nsuchar> nsboolucharmultimap;
typedef std::multimap<nsbool, nsusint> nsboolusintmultimap;
typedef std::multimap<nsbool, nsuint> nsbooluintmultimap;
typedef std::multimap<nsbool, nsulint> nsboolulintmultimap;
typedef std::multimap<nsbool, nsullint> nsboolullintmultimap;
typedef std::multimap<nsbool, nsfloat> nsboolfloatmultimap;
typedef std::multimap<nsbool, nsdouble> nsbooldoublemultimap;
typedef std::multimap<nsbool, nsldouble> nsboolldoublemultimap;
typedef std::multimap<nsbool, nsbool> nsboolboolmultimap;
typedef std::multimap<nsbool, nsstring> nsboolstringmultimap;
typedef std::multimap<nsstring, nschar> nsstringcharmultimap;
typedef std::multimap<nsstring, nschar16> nsstringchar16multimap;
typedef std::multimap<nsstring, nschar32> nsstringchar32multimap;
typedef std::multimap<nsstring, nswchar> nsstringwcharmultimap;
typedef std::multimap<nsstring, nssint> nsstringsintmultimap;
typedef std::multimap<nsstring, nsint> nsstringintmultimap;
typedef std::multimap<nsstring, nslint> nsstringlintmultimap;
typedef std::multimap<nsstring, nsllint> nsstringllintmultimap;
typedef std::multimap<nsstring, nsuchar> nsstringucharmultimap;
typedef std::multimap<nsstring, nsusint> nsstringusintmultimap;
typedef std::multimap<nsstring, nsuint> nsstringuintmultimap;
typedef std::multimap<nsstring, nsulint> nsstringulintmultimap;
typedef std::multimap<nsstring, nsullint> nsstringullintmultimap;
typedef std::multimap<nsstring, nsfloat> nsstringfloatmultimap;
typedef std::multimap<nsstring, nsdouble> nsstringdoublemultimap;
typedef std::multimap<nsstring, nsldouble> nsstringldoublemultimap;
typedef std::multimap<nsstring, nsbool> nsstringboolmultimap;
typedef std::multimap<nsstring, nsstring> nsstringstringmultimap;

class NSMaterial;
class NSEntity;

typedef std::set<NSMaterial*> nspmatset;
typedef std::map<nsuint, NSEntity*> nsuintpentity_map;
typedef std::set<NSEntity*> nspentityset;

GLEWContext * glewGetContext();
bool GLError(nsstring errorMessage);
nsuint HashedStringID(const nsstring & string);

#endif
