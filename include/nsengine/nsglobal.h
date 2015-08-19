
#ifndef GLOBAL_H
#define GLOBAL_H

//#ifdef _DEBUG
#define NSDEBUG
#define NSDEBUG_RT
//#endif

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
#define ANIM_TYPESTRING "nsanim_set"
#define ENTITY_TYPESTRING "nsentity"
#define MATERIAL_TYPESTRING "nsmaterial"
#define MESH_TYPESTRING "nsmesh"
#define INPUTMAP_TYPESTRING "nsinput_map"
#define TEX1D_TYPESTRING "nstex1d"
#define TEX1DARRAY_TYPESTRING "nstex1d_array"
#define TEX2D_TYPESTRING "nstex2d"
#define TEXRECTANGLE_TYPESTRING "nstex_rectangle"
#define TEX2DMULTISAMPLE_TYPESTRING "nstex2d_multisample"
#define TEX2DMULTISAMPLEARRAY_TYPESTRING "nstex2d_multisample_array"
#define TEXBUFFER_TYPESTRING "nstex_buffer"
#define TEX2DARRAY_TYPESTRING "nstex2d_array"
#define TEX3D_TYPESTRING "nstex3d"
#define TEXCUBEMAP_TYPESTRING "nstex_cubemap"
#define TEXCUBEMAPARRAY_TYPESTRING "nstex_cubemap_array"
#define SHADER_TYPESTRING "nsshader"
#define DIRLIGHTSHADER_TYPESTRING "nsdir_light_shader"
#define POINTLIGHTSHADER_TYPESTRING "nspoint_light_shader"
#define SPOTLIGHTSHADER_TYPESTRING "nsspot_light_shader"
#define MATERIALSHADER_TYPESTRING "nsmaterial_shader"
#define PARTICLEPROCESSSHADER_TYPESTRING "nsparticle_process_shader"
#define PARTICLERENDERSHADER_TYPESTRING "nsparticle_render_shader"
#define DIR_SHADOWMAP_SHADER_TYPESTRING "nsdir_shadowmap_shader"
#define DIR_SHADOWMAPXFB_SHADER_TYPESTRING "nsdir_shadowmap_xfb_shader"
#define POINT_SHADOWMAP_SHADER_TYPESTRING "nspoint_shadowmap_shader"
#define POINT_SHADOWMAPXFB_SHADER_TYPESTRING "nspoint_shadowmap_xfb_shader"
#define SPOT_SHADOWMAP_SHADER_TYPESTRING "nsspot_shadowmap_shader"
#define SPOT_SHADOWMAPXFB_SHADER_TYPESTRING "nsspot_shadowmap_xfb_shader"
#define EARLYZ_SHADER_TYPESTRING "nsearlyz_shader"
#define EARLYZXFB_SHADER_TYPESTRING "nsearlyz_xfb_shader"
#define RENDERXFB_SHADER_TYPESTRING "nsrender_xfb_shader"
#define XFB_SHADER_TYPESTRING "nsxfb_shader"
#define LIGHTSTENCIL_SHADER_TYPESTRING "nslight_stencil_shader"
#define SKYBOX_SHADER_TYPESTRING "nsskybox_shader"
#define TRANSPARENCY_SHADER_TYPESTRING "nstransparency_shader"
#define SELECTION_SHADER_TYPESTRING "nsselection_shader"
#define SCENE_TYPESTRING "nsscene"
#define PLUGIN_TYPESTRING "nsplugin"

#define ENGINE_PLUG "engplug"

#define ANIM_MANAGER_TYPESTRING "nsanim_manager"
#define ENTITY_MANAGER_TYPESTRING "nsentity_manager"
#define MAT_MANAGER_TYPESTRING "nsmat_manager"
#define MESH_MANAGER_TYPESTRING "nsmesh_manager"
#define TEX_MANAGER_TYPESTRING "nstex_manager"
#define SHADER_MANAGER_TYPESTRING "nsshader_manager"
#define SCENE_MANAGER_TYPESTRING "nsscene_manager"
#define PLUGIN_MANAGER_TYPESTRING "nsplugin_manager"
#define INPUTMAP_MANAGER_TYPESTRING "nsinput_map_manager"

// Component Typenames
#define ANIM_COMP_TYPESTRING "nsanim_comp"
#define CAM_COMP_TYPESTRING "nscam_comp"
#define LIGHT_COMP_TYPESTRING "nslight_comp"
#define OCCUPY_COMP_TYPESTRING "nsoccupy_comp"
#define RENDER_COMP_TYPESTRING "nsrender_comp"
#define SEL_COMP_TYPESTRING "nssel_comp"
#define TFORM_COMP_TYPESTRING "nstform_comp"
#define TILEBRUSH_COMP_TYPESTRING "nstile_brush_comp"
#define TILE_COMP_TYPESTRING "nstile_comp"
#define PARTICLE_COMP_TYPESTRING "nsparticle_comp"
#define TERRAIN_COMP_TYPESTRING "nsterrain_comp"

// System Typenames
#define ANIM_SYS_TYPESTRING "nsanim_system"
#define CAM_SYS_TYPESTRING "nscamera_system"
#define BUILD_SYS_TYPESTRING "nsbuild_system"
#define MOVE_SYS_TYPESTRING "nsmovement_system"
#define RENDER_SYS_TYPESTRING "nsrender_system"
#define SEL_SYS_TYPESTRING "nsselection_system"
#define UI_SYS_TYPESTRING "NSUISystem"
#define INP_SYS_TYPESTRING "nsinput_system"
#define PARTICLE_SYS_TYPESTRING "nsparticle_system"

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


#define COMMENT SLASH(/)
#define SLASH(s) /##s

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

struct EnumHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

#include <nstypes.h>

// Basic type typedefs

typedef std::string nsstring;
typedef std::stringstream nsstringstream;
typedef std::fstream nsfstream;

extern uint32 hash_id(const nsstring &);

typedef std::vector<char> nschararray;
typedef std::vector<char16> char16array;
typedef std::vector<char32> char32array;
typedef std::vector<wchar> wchararray;
typedef std::vector<int16> int16array;
typedef std::vector<int32> int32array;
typedef std::vector<int32> int32array;
typedef std::vector<int64> int64array;
typedef std::vector<uint8> uint8array;
typedef std::vector<uint16> uint16array;
typedef std::vector<uint32> uint32array;
typedef std::vector<uint32> uint32array;
typedef std::vector<uint64> uint64array;
typedef std::vector<float> floatarray;
typedef std::vector<double> doublearray;
typedef std::vector<ldouble> ldoublearray;
typedef std::vector<bool> boolarray;
typedef std::vector<nsstring> nsstringarray;

typedef std::set<char> nscharset;
typedef std::set<char16> char16set;
typedef std::set<char32> char32set;
typedef std::set<wchar> wcharset;
typedef std::set<int16> int16set;
typedef std::set<int32> int32set;
typedef std::set<int32> int32set;
typedef std::set<int64> int64set;
typedef std::set<uint8> uint8set;
typedef std::set<uint16> uint16set;
typedef std::set<uint32> uint32set;
typedef std::set<uint32> uint32set;
typedef std::set<uint64> uint64set;
typedef std::set<float> floatset;
typedef std::set<double> doubleset;
typedef std::set<ldouble> ldoubleset;
typedef std::set<bool> boolset;
typedef std::set<nsstring> nsstringset;

typedef std::unordered_set<char> nscharu_set;
typedef std::unordered_set<char16> char16u_set;
typedef std::unordered_set<char32> char32u_set;
typedef std::unordered_set<wchar> wcharu_set;
typedef std::unordered_set<int16> int16u_set;
typedef std::unordered_set<int32> int32u_set;
typedef std::unordered_set<int32> int32u_set;
typedef std::unordered_set<int64> int64u_set;
typedef std::unordered_set<uint8> uint8u_set;
typedef std::unordered_set<uint16> uint16u_set;
typedef std::unordered_set<uint32> uint32u_set;
typedef std::unordered_set<uint32> uint32u_set;
typedef std::unordered_set<uint64> uint64u_set;
typedef std::unordered_set<float> floatu_set;
typedef std::unordered_set<double> doubleu_set;
typedef std::unordered_set<ldouble> ldoubleu_set;
typedef std::unordered_set<bool> boolu_set;
typedef std::unordered_set<nsstring> nsstringu_set;

typedef std::multiset<char> nscharmultiset;
typedef std::multiset<char16> char16multiset;
typedef std::multiset<char32> char32multiset;
typedef std::multiset<wchar> wcharmultiset;
typedef std::multiset<int16> int16multiset;
typedef std::multiset<int32> int32multiset;
typedef std::multiset<int32> int32multiset;
typedef std::multiset<int64> int64multiset;
typedef std::multiset<uint8> uint8multiset;
typedef std::multiset<uint16> uint16multiset;
typedef std::multiset<uint32> uint32multiset;
typedef std::multiset<uint32> uint32multiset;
typedef std::multiset<uint64> uint64multiset;
typedef std::multiset<float> floatmultiset;
typedef std::multiset<double> doublemultiset;
typedef std::multiset<ldouble> ldoublemultiset;
typedef std::multiset<bool> boolmultiset;
typedef std::multiset<nsstring> nsstringmultiset;

typedef std::map<char, char> nscharcharmap;
typedef std::map<char, char16> nscharchar16map;
typedef std::map<char, char32> nscharchar32map;
typedef std::map<char, wchar> nscharwcharmap;
typedef std::map<char, int16> nscharsintmap;
typedef std::map<char, int32> nscharintmap;
typedef std::map<char, int32> nscharlintmap;
typedef std::map<char, int64> nscharllintmap;
typedef std::map<char, uint8> nscharucharmap;
typedef std::map<char, uint16> nscharusintmap;
typedef std::map<char, uint32> nscharuintmap;
typedef std::map<char, uint32> nscharulintmap;
typedef std::map<char, uint64> nscharullintmap;
typedef std::map<char, float> nscharfloatmap;
typedef std::map<char, double> nschardoublemap;
typedef std::map<char, ldouble> nscharldoublemap;
typedef std::map<char, bool> nscharboolmap;
typedef std::map<char, nsstring> nscharstringmap;
typedef std::map<char16, char> char16charmap;
typedef std::map<char16, char16> char16char16map;
typedef std::map<char16, char32> char16char32map;
typedef std::map<char16, wchar> char16wcharmap;
typedef std::map<char16, int16> char16sintmap;
typedef std::map<char16, int32> char16intmap;
typedef std::map<char16, int32> char16lintmap;
typedef std::map<char16, int64> char16llintmap;
typedef std::map<char16, uint8> char16ucharmap;
typedef std::map<char16, uint16> char16usintmap;
typedef std::map<char16, uint32> char16uintmap;
typedef std::map<char16, uint32> char16ulintmap;
typedef std::map<char16, uint64> char16ullintmap;
typedef std::map<char16, float> char16floatmap;
typedef std::map<char16, double> char16doublemap;
typedef std::map<char16, ldouble> char16ldoublemap;
typedef std::map<char16, bool> char16boolmap;
typedef std::map<char16, nsstring> char16stringmap;
typedef std::map<char32, char> char32charmap;
typedef std::map<char32, char16> char32char16map;
typedef std::map<char32, char32> char32char32map;
typedef std::map<char32, wchar> char32wcharmap;
typedef std::map<char32, int16> char32sintmap;
typedef std::map<char32, int32> char32intmap;
typedef std::map<char32, int32> char32lintmap;
typedef std::map<char32, int64> char32llintmap;
typedef std::map<char32, uint8> char32ucharmap;
typedef std::map<char32, uint16> char32usintmap;
typedef std::map<char32, uint32> char32uintmap;
typedef std::map<char32, uint32> char32ulintmap;
typedef std::map<char32, uint64> char32ullintmap;
typedef std::map<char32, float> char32floatmap;
typedef std::map<char32, double> char32doublemap;
typedef std::map<char32, ldouble> char32ldoublemap;
typedef std::map<char32, bool> char32boolmap;
typedef std::map<char32, nsstring> char32stringmap;
typedef std::map<wchar, char> wcharcharmap;
typedef std::map<wchar, char16> wcharchar16map;
typedef std::map<wchar, char32> wcharchar32map;
typedef std::map<wchar, wchar> wcharwcharmap;
typedef std::map<wchar, int16> wcharsintmap;
typedef std::map<wchar, int32> wcharintmap;
typedef std::map<wchar, int32> wcharlintmap;
typedef std::map<wchar, int64> wcharllintmap;
typedef std::map<wchar, uint8> wcharucharmap;
typedef std::map<wchar, uint16> wcharusintmap;
typedef std::map<wchar, uint32> wcharuintmap;
typedef std::map<wchar, uint32> wcharulintmap;
typedef std::map<wchar, uint64> wcharullintmap;
typedef std::map<wchar, float> wcharfloatmap;
typedef std::map<wchar, double> wchardoublemap;
typedef std::map<wchar, ldouble> wcharldoublemap;
typedef std::map<wchar, bool> wcharboolmap;
typedef std::map<wchar, nsstring> wcharstringmap;
typedef std::map<int16, char> int16charmap;
typedef std::map<int16, char16> int16char16map;
typedef std::map<int16, char32> int16char32map;
typedef std::map<int16, wchar> int16wcharmap;
typedef std::map<int16, int16> int16sintmap;
typedef std::map<int16, int32> int16intmap;
typedef std::map<int16, int32> int16lintmap;
typedef std::map<int16, int64> int16llintmap;
typedef std::map<int16, uint8> int16ucharmap;
typedef std::map<int16, uint16> int16usintmap;
typedef std::map<int16, uint32> int16uintmap;
typedef std::map<int16, uint32> int16ulintmap;
typedef std::map<int16, uint64> int16ullintmap;
typedef std::map<int16, float> int16floatmap;
typedef std::map<int16, double> int16doublemap;
typedef std::map<int16, ldouble> int16ldoublemap;
typedef std::map<int16, bool> int16boolmap;
typedef std::map<int16, nsstring> int16stringmap;
typedef std::map<int32, char> int32charmap;
typedef std::map<int32, char16> int32char16map;
typedef std::map<int32, char32> int32char32map;
typedef std::map<int32, wchar> int32wcharmap;
typedef std::map<int32, int16> int32sintmap;
typedef std::map<int32, int32> int32intmap;
typedef std::map<int32, int32> int32lintmap;
typedef std::map<int32, int64> int32llintmap;
typedef std::map<int32, uint8> int32ucharmap;
typedef std::map<int32, uint16> int32usintmap;
typedef std::map<int32, uint32> int32uintmap;
typedef std::map<int32, uint32> int32ulintmap;
typedef std::map<int32, uint64> int32ullintmap;
typedef std::map<int32, float> int32floatmap;
typedef std::map<int32, double> int32doublemap;
typedef std::map<int32, ldouble> int32ldoublemap;
typedef std::map<int32, bool> int32boolmap;
typedef std::map<int32, nsstring> int32stringmap;
typedef std::map<int32, char> int32charmap;
typedef std::map<int32, char16> int32char16map;
typedef std::map<int32, char32> int32char32map;
typedef std::map<int32, wchar> int32wcharmap;
typedef std::map<int32, int16> int32sintmap;
typedef std::map<int32, int32> int32intmap;
typedef std::map<int32, int32> int32lintmap;
typedef std::map<int32, int64> int32llintmap;
typedef std::map<int32, uint8> int32ucharmap;
typedef std::map<int32, uint16> int32usintmap;
typedef std::map<int32, uint32> int32uintmap;
typedef std::map<int32, uint32> int32ulintmap;
typedef std::map<int32, uint64> int32ullintmap;
typedef std::map<int32, float> int32floatmap;
typedef std::map<int32, double> int32doublemap;
typedef std::map<int32, ldouble> int32ldoublemap;
typedef std::map<int32, bool> int32boolmap;
typedef std::map<int32, nsstring> int32stringmap;
typedef std::map<int64, char> int64charmap;
typedef std::map<int64, char16> int64char16map;
typedef std::map<int64, char32> int64char32map;
typedef std::map<int64, wchar> int64wcharmap;
typedef std::map<int64, int16> int64sintmap;
typedef std::map<int64, int32> int64intmap;
typedef std::map<int64, int32> int64lintmap;
typedef std::map<int64, int64> int64llintmap;
typedef std::map<int64, uint8> int64ucharmap;
typedef std::map<int64, uint16> int64usintmap;
typedef std::map<int64, uint32> int64uintmap;
typedef std::map<int64, uint32> int64ulintmap;
typedef std::map<int64, uint64> int64ullintmap;
typedef std::map<int64, float> int64floatmap;
typedef std::map<int64, double> int64doublemap;
typedef std::map<int64, ldouble> int64ldoublemap;
typedef std::map<int64, bool> int64boolmap;
typedef std::map<int64, nsstring> int64stringmap;
typedef std::map<uint8, char> uint8charmap;
typedef std::map<uint8, char16> uint8char16map;
typedef std::map<uint8, char32> uint8char32map;
typedef std::map<uint8, wchar> uint8wcharmap;
typedef std::map<uint8, int16> uint8sintmap;
typedef std::map<uint8, int32> uint8intmap;
typedef std::map<uint8, int32> uint8lintmap;
typedef std::map<uint8, int64> uint8llintmap;
typedef std::map<uint8, uint8> uint8ucharmap;
typedef std::map<uint8, uint16> uint8usintmap;
typedef std::map<uint8, uint32> uint8uintmap;
typedef std::map<uint8, uint32> uint8ulintmap;
typedef std::map<uint8, uint64> uint8ullintmap;
typedef std::map<uint8, float> uint8floatmap;
typedef std::map<uint8, double> uint8doublemap;
typedef std::map<uint8, ldouble> uint8ldoublemap;
typedef std::map<uint8, bool> uint8boolmap;
typedef std::map<uint8, nsstring> uint8stringmap;
typedef std::map<uint16, char> uint16charmap;
typedef std::map<uint16, char16> uint16char16map;
typedef std::map<uint16, char32> uint16char32map;
typedef std::map<uint16, wchar> uint16wcharmap;
typedef std::map<uint16, int16> uint16sintmap;
typedef std::map<uint16, int32> uint16intmap;
typedef std::map<uint16, int32> uint16lintmap;
typedef std::map<uint16, int64> uint16llintmap;
typedef std::map<uint16, uint8> uint16ucharmap;
typedef std::map<uint16, uint16> uint16usintmap;
typedef std::map<uint16, uint32> uint16uintmap;
typedef std::map<uint16, uint32> uint16ulintmap;
typedef std::map<uint16, uint64> uint16ullintmap;
typedef std::map<uint16, float> uint16floatmap;
typedef std::map<uint16, double> uint16doublemap;
typedef std::map<uint16, ldouble> uint16ldoublemap;
typedef std::map<uint16, bool> uint16boolmap;
typedef std::map<uint16, nsstring> uint16stringmap;
typedef std::map<uint32, char> uint32charmap;
typedef std::map<uint32, char16> uint32char16map;
typedef std::map<uint32, char32> uint32char32map;
typedef std::map<uint32, wchar> uint32wcharmap;
typedef std::map<uint32, int16> uint32sintmap;
typedef std::map<uint32, int32> uint32intmap;
typedef std::map<uint32, int32> uint32lintmap;
typedef std::map<uint32, int64> uint32llintmap;
typedef std::map<uint32, uint8> uint32ucharmap;
typedef std::map<uint32, uint16> uint32usintmap;
typedef std::map<uint32, uint32> uint32uintmap;
typedef std::map<uint32, uint32> uint32ulintmap;
typedef std::map<uint32, uint64> uint32ullintmap;
typedef std::map<uint32, float> uint32floatmap;
typedef std::map<uint32, double> uint32doublemap;
typedef std::map<uint32, ldouble> uint32ldoublemap;
typedef std::map<uint32, bool> uint32boolmap;
typedef std::map<uint32, nsstring> uint32stringmap;
typedef std::map<uint32, char> uint32charmap;
typedef std::map<uint32, char16> uint32char16map;
typedef std::map<uint32, char32> uint32char32map;
typedef std::map<uint32, wchar> uint32wcharmap;
typedef std::map<uint32, int16> uint32sintmap;
typedef std::map<uint32, int32> uint32intmap;
typedef std::map<uint32, int32> uint32lintmap;
typedef std::map<uint32, int64> uint32llintmap;
typedef std::map<uint32, uint8> uint32ucharmap;
typedef std::map<uint32, uint16> uint32usintmap;
typedef std::map<uint32, uint32> uint32uintmap;
typedef std::map<uint32, uint32> uint32ulintmap;
typedef std::map<uint32, uint64> uint32ullintmap;
typedef std::map<uint32, float> uint32floatmap;
typedef std::map<uint32, double> uint32doublemap;
typedef std::map<uint32, ldouble> uint32ldoublemap;
typedef std::map<uint32, bool> uint32boolmap;
typedef std::map<uint32, nsstring> uint32stringmap;
typedef std::map<uint64, char> uint64charmap;
typedef std::map<uint64, char16> uint64char16map;
typedef std::map<uint64, char32> uint64char32map;
typedef std::map<uint64, wchar> uint64wcharmap;
typedef std::map<uint64, int16> uint64sintmap;
typedef std::map<uint64, int32> uint64intmap;
typedef std::map<uint64, int32> uint64lintmap;
typedef std::map<uint64, int64> uint64llintmap;
typedef std::map<uint64, uint8> uint64ucharmap;
typedef std::map<uint64, uint16> uint64usintmap;
typedef std::map<uint64, uint32> uint64uintmap;
typedef std::map<uint64, uint32> uint64ulintmap;
typedef std::map<uint64, uint64> uint64ullintmap;
typedef std::map<uint64, float> uint64floatmap;
typedef std::map<uint64, double> uint64doublemap;
typedef std::map<uint64, ldouble> uint64ldoublemap;
typedef std::map<uint64, bool> uint64boolmap;
typedef std::map<uint64, nsstring> uint64stringmap;
typedef std::map<float, char> floatcharmap;
typedef std::map<float, char16> floatchar16map;
typedef std::map<float, char32> floatchar32map;
typedef std::map<float, wchar> floatwcharmap;
typedef std::map<float, int16> floatsintmap;
typedef std::map<float, int32> floatintmap;
typedef std::map<float, int32> floatlintmap;
typedef std::map<float, int64> floatllintmap;
typedef std::map<float, uint8> floatucharmap;
typedef std::map<float, uint16> floatusintmap;
typedef std::map<float, uint32> floatuintmap;
typedef std::map<float, uint32> floatulintmap;
typedef std::map<float, uint64> floatullintmap;
typedef std::map<float, float> floatfloatmap;
typedef std::map<float, double> floatdoublemap;
typedef std::map<float, ldouble> floatldoublemap;
typedef std::map<float, bool> floatboolmap;
typedef std::map<float, nsstring> floatstringmap;
typedef std::map<double, char> doublecharmap;
typedef std::map<double, char16> doublechar16map;
typedef std::map<double, char32> doublechar32map;
typedef std::map<double, wchar> doublewcharmap;
typedef std::map<double, int16> doublesintmap;
typedef std::map<double, int32> doubleintmap;
typedef std::map<double, int32> doublelintmap;
typedef std::map<double, int64> doublellintmap;
typedef std::map<double, uint8> doubleucharmap;
typedef std::map<double, uint16> doubleusintmap;
typedef std::map<double, uint32> doubleuintmap;
typedef std::map<double, uint32> doubleulintmap;
typedef std::map<double, uint64> doubleullintmap;
typedef std::map<double, float> doublefloatmap;
typedef std::map<double, double> doubledoublemap;
typedef std::map<double, ldouble> doubleldoublemap;
typedef std::map<double, bool> doubleboolmap;
typedef std::map<double, nsstring> doublestringmap;
typedef std::map<ldouble, char> ldoublecharmap;
typedef std::map<ldouble, char16> ldoublechar16map;
typedef std::map<ldouble, char32> ldoublechar32map;
typedef std::map<ldouble, wchar> ldoublewcharmap;
typedef std::map<ldouble, int16> ldoublesintmap;
typedef std::map<ldouble, int32> ldoubleintmap;
typedef std::map<ldouble, int32> ldoublelintmap;
typedef std::map<ldouble, int64> ldoublellintmap;
typedef std::map<ldouble, uint8> ldoubleucharmap;
typedef std::map<ldouble, uint16> ldoubleusintmap;
typedef std::map<ldouble, uint32> ldoubleuintmap;
typedef std::map<ldouble, uint32> ldoubleulintmap;
typedef std::map<ldouble, uint64> ldoubleullintmap;
typedef std::map<ldouble, float> ldoublefloatmap;
typedef std::map<ldouble, double> ldoubledoublemap;
typedef std::map<ldouble, ldouble> ldoubleldoublemap;
typedef std::map<ldouble, bool> ldoubleboolmap;
typedef std::map<ldouble, nsstring> ldoublestringmap;
typedef std::map<bool, char> boolcharmap;
typedef std::map<bool, char16> boolchar16map;
typedef std::map<bool, char32> boolchar32map;
typedef std::map<bool, wchar> boolwcharmap;
typedef std::map<bool, int16> boolsintmap;
typedef std::map<bool, int32> boolintmap;
typedef std::map<bool, int32> boollintmap;
typedef std::map<bool, int64> boolllintmap;
typedef std::map<bool, uint8> boolucharmap;
typedef std::map<bool, uint16> boolusintmap;
typedef std::map<bool, uint32> booluintmap;
typedef std::map<bool, uint32> boolulintmap;
typedef std::map<bool, uint64> boolullintmap;
typedef std::map<bool, float> boolfloatmap;
typedef std::map<bool, double> booldoublemap;
typedef std::map<bool, ldouble> boolldoublemap;
typedef std::map<bool, bool> boolboolmap;
typedef std::map<bool, nsstring> boolstringmap;
typedef std::map<nsstring, char> nsstringcharmap;
typedef std::map<nsstring, char16> nsstringchar16map;
typedef std::map<nsstring, char32> nsstringchar32map;
typedef std::map<nsstring, wchar> nsstringwcharmap;
typedef std::map<nsstring, int16> nsstringsintmap;
typedef std::map<nsstring, int32> nsstringintmap;
typedef std::map<nsstring, int32> nsstringlintmap;
typedef std::map<nsstring, int64> nsstringllintmap;
typedef std::map<nsstring, uint8> nsstringucharmap;
typedef std::map<nsstring, uint16> nsstringusintmap;
typedef std::map<nsstring, uint32> nsstringuintmap;
typedef std::map<nsstring, uint32> nsstringulintmap;
typedef std::map<nsstring, uint64> nsstringullintmap;
typedef std::map<nsstring, float> nsstringfloatmap;
typedef std::map<nsstring, double> nsstringdoublemap;
typedef std::map<nsstring, ldouble> nsstringldoublemap;
typedef std::map<nsstring, bool> nsstringboolmap;
typedef std::map<nsstring, nsstring> nsstringstringmap;

typedef std::unordered_map<char, char> nscharcharu_map;
typedef std::unordered_map<char, char16> nscharchar16u_map;
typedef std::unordered_map<char, char32> nscharchar32u_map;
typedef std::unordered_map<char, wchar> nscharwcharu_map;
typedef std::unordered_map<char, int16> nscharsintu_map;
typedef std::unordered_map<char, int32> nscharintu_map;
typedef std::unordered_map<char, int32> nscharlintu_map;
typedef std::unordered_map<char, int64> nscharllintu_map;
typedef std::unordered_map<char, uint8> nscharucharu_map;
typedef std::unordered_map<char, uint16> nscharusintu_map;
typedef std::unordered_map<char, uint32> nscharuintu_map;
typedef std::unordered_map<char, uint32> nscharulintu_map;
typedef std::unordered_map<char, uint64> nscharullintu_map;
typedef std::unordered_map<char, float> nscharfloatu_map;
typedef std::unordered_map<char, double> nschardoubleu_map;
typedef std::unordered_map<char, ldouble> nscharldoubleu_map;
typedef std::unordered_map<char, bool> nscharboolu_map;
typedef std::unordered_map<char, nsstring> nscharstringu_map;
typedef std::unordered_map<char16, char> char16charu_map;
typedef std::unordered_map<char16, char16> char16char16u_map;
typedef std::unordered_map<char16, char32> char16char32u_map;
typedef std::unordered_map<char16, wchar> char16wcharu_map;
typedef std::unordered_map<char16, int16> char16sintu_map;
typedef std::unordered_map<char16, int32> char16intu_map;
typedef std::unordered_map<char16, int32> char16lintu_map;
typedef std::unordered_map<char16, int64> char16llintu_map;
typedef std::unordered_map<char16, uint8> char16ucharu_map;
typedef std::unordered_map<char16, uint16> char16usintu_map;
typedef std::unordered_map<char16, uint32> char16uintu_map;
typedef std::unordered_map<char16, uint32> char16ulintu_map;
typedef std::unordered_map<char16, uint64> char16ullintu_map;
typedef std::unordered_map<char16, float> char16floatu_map;
typedef std::unordered_map<char16, double> char16doubleu_map;
typedef std::unordered_map<char16, ldouble> char16ldoubleu_map;
typedef std::unordered_map<char16, bool> char16boolu_map;
typedef std::unordered_map<char16, nsstring> char16stringu_map;
typedef std::unordered_map<char32, char> char32charu_map;
typedef std::unordered_map<char32, char16> char32char16u_map;
typedef std::unordered_map<char32, char32> char32char32u_map;
typedef std::unordered_map<char32, wchar> char32wcharu_map;
typedef std::unordered_map<char32, int16> char32sintu_map;
typedef std::unordered_map<char32, int32> char32intu_map;
typedef std::unordered_map<char32, int32> char32lintu_map;
typedef std::unordered_map<char32, int64> char32llintu_map;
typedef std::unordered_map<char32, uint8> char32ucharu_map;
typedef std::unordered_map<char32, uint16> char32usintu_map;
typedef std::unordered_map<char32, uint32> char32uintu_map;
typedef std::unordered_map<char32, uint32> char32ulintu_map;
typedef std::unordered_map<char32, uint64> char32ullintu_map;
typedef std::unordered_map<char32, float> char32floatu_map;
typedef std::unordered_map<char32, double> char32doubleu_map;
typedef std::unordered_map<char32, ldouble> char32ldoubleu_map;
typedef std::unordered_map<char32, bool> char32boolu_map;
typedef std::unordered_map<char32, nsstring> char32stringu_map;
typedef std::unordered_map<wchar, char> wcharcharu_map;
typedef std::unordered_map<wchar, char16> wcharchar16u_map;
typedef std::unordered_map<wchar, char32> wcharchar32u_map;
typedef std::unordered_map<wchar, wchar> wcharwcharu_map;
typedef std::unordered_map<wchar, int16> wcharsintu_map;
typedef std::unordered_map<wchar, int32> wcharintu_map;
typedef std::unordered_map<wchar, int32> wcharlintu_map;
typedef std::unordered_map<wchar, int64> wcharllintu_map;
typedef std::unordered_map<wchar, uint8> wcharucharu_map;
typedef std::unordered_map<wchar, uint16> wcharusintu_map;
typedef std::unordered_map<wchar, uint32> wcharuintu_map;
typedef std::unordered_map<wchar, uint32> wcharulintu_map;
typedef std::unordered_map<wchar, uint64> wcharullintu_map;
typedef std::unordered_map<wchar, float> wcharfloatu_map;
typedef std::unordered_map<wchar, double> wchardoubleu_map;
typedef std::unordered_map<wchar, ldouble> wcharldoubleu_map;
typedef std::unordered_map<wchar, bool> wcharboolu_map;
typedef std::unordered_map<wchar, nsstring> wcharstringu_map;
typedef std::unordered_map<int16, char> int16charu_map;
typedef std::unordered_map<int16, char16> int16char16u_map;
typedef std::unordered_map<int16, char32> int16char32u_map;
typedef std::unordered_map<int16, wchar> int16wcharu_map;
typedef std::unordered_map<int16, int16> int16sintu_map;
typedef std::unordered_map<int16, int32> int16intu_map;
typedef std::unordered_map<int16, int32> int16lintu_map;
typedef std::unordered_map<int16, int64> int16llintu_map;
typedef std::unordered_map<int16, uint8> int16ucharu_map;
typedef std::unordered_map<int16, uint16> int16usintu_map;
typedef std::unordered_map<int16, uint32> int16uintu_map;
typedef std::unordered_map<int16, uint32> int16ulintu_map;
typedef std::unordered_map<int16, uint64> int16ullintu_map;
typedef std::unordered_map<int16, float> int16floatu_map;
typedef std::unordered_map<int16, double> int16doubleu_map;
typedef std::unordered_map<int16, ldouble> int16ldoubleu_map;
typedef std::unordered_map<int16, bool> int16boolu_map;
typedef std::unordered_map<int16, nsstring> int16stringu_map;
typedef std::unordered_map<int32, char> int32charu_map;
typedef std::unordered_map<int32, char16> int32char16u_map;
typedef std::unordered_map<int32, char32> int32char32u_map;
typedef std::unordered_map<int32, wchar> int32wcharu_map;
typedef std::unordered_map<int32, int16> int32sintu_map;
typedef std::unordered_map<int32, int32> int32intu_map;
typedef std::unordered_map<int32, int32> int32lintu_map;
typedef std::unordered_map<int32, int64> int32llintu_map;
typedef std::unordered_map<int32, uint8> int32ucharu_map;
typedef std::unordered_map<int32, uint16> int32usintu_map;
typedef std::unordered_map<int32, uint32> int32uintu_map;
typedef std::unordered_map<int32, uint32> int32ulintu_map;
typedef std::unordered_map<int32, uint64> int32ullintu_map;
typedef std::unordered_map<int32, float> int32floatu_map;
typedef std::unordered_map<int32, double> int32doubleu_map;
typedef std::unordered_map<int32, ldouble> int32ldoubleu_map;
typedef std::unordered_map<int32, bool> int32boolu_map;
typedef std::unordered_map<int32, nsstring> int32stringu_map;
typedef std::unordered_map<int32, char> int32charu_map;
typedef std::unordered_map<int32, char16> int32char16u_map;
typedef std::unordered_map<int32, char32> int32char32u_map;
typedef std::unordered_map<int32, wchar> int32wcharu_map;
typedef std::unordered_map<int32, int16> int32sintu_map;
typedef std::unordered_map<int32, int32> int32intu_map;
typedef std::unordered_map<int32, int32> int32lintu_map;
typedef std::unordered_map<int32, int64> int32llintu_map;
typedef std::unordered_map<int32, uint8> int32ucharu_map;
typedef std::unordered_map<int32, uint16> int32usintu_map;
typedef std::unordered_map<int32, uint32> int32uintu_map;
typedef std::unordered_map<int32, uint32> int32ulintu_map;
typedef std::unordered_map<int32, uint64> int32ullintu_map;
typedef std::unordered_map<int32, float> int32floatu_map;
typedef std::unordered_map<int32, double> int32doubleu_map;
typedef std::unordered_map<int32, ldouble> int32ldoubleu_map;
typedef std::unordered_map<int32, bool> int32boolu_map;
typedef std::unordered_map<int32, nsstring> int32stringu_map;
typedef std::unordered_map<int64, char> int64charu_map;
typedef std::unordered_map<int64, char16> int64char16u_map;
typedef std::unordered_map<int64, char32> int64char32u_map;
typedef std::unordered_map<int64, wchar> int64wcharu_map;
typedef std::unordered_map<int64, int16> int64sintu_map;
typedef std::unordered_map<int64, int32> int64intu_map;
typedef std::unordered_map<int64, int32> int64lintu_map;
typedef std::unordered_map<int64, int64> int64llintu_map;
typedef std::unordered_map<int64, uint8> int64ucharu_map;
typedef std::unordered_map<int64, uint16> int64usintu_map;
typedef std::unordered_map<int64, uint32> int64uintu_map;
typedef std::unordered_map<int64, uint32> int64ulintu_map;
typedef std::unordered_map<int64, uint64> int64ullintu_map;
typedef std::unordered_map<int64, float> int64floatu_map;
typedef std::unordered_map<int64, double> int64doubleu_map;
typedef std::unordered_map<int64, ldouble> int64ldoubleu_map;
typedef std::unordered_map<int64, bool> int64boolu_map;
typedef std::unordered_map<int64, nsstring> int64stringu_map;
typedef std::unordered_map<uint8, char> uint8charu_map;
typedef std::unordered_map<uint8, char16> uint8char16u_map;
typedef std::unordered_map<uint8, char32> uint8char32u_map;
typedef std::unordered_map<uint8, wchar> uint8wcharu_map;
typedef std::unordered_map<uint8, int16> uint8sintu_map;
typedef std::unordered_map<uint8, int32> uint8intu_map;
typedef std::unordered_map<uint8, int32> uint8lintu_map;
typedef std::unordered_map<uint8, int64> uint8llintu_map;
typedef std::unordered_map<uint8, uint8> uint8ucharu_map;
typedef std::unordered_map<uint8, uint16> uint8usintu_map;
typedef std::unordered_map<uint8, uint32> uint8uintu_map;
typedef std::unordered_map<uint8, uint32> uint8ulintu_map;
typedef std::unordered_map<uint8, uint64> uint8ullintu_map;
typedef std::unordered_map<uint8, float> uint8floatu_map;
typedef std::unordered_map<uint8, double> uint8doubleu_map;
typedef std::unordered_map<uint8, ldouble> uint8ldoubleu_map;
typedef std::unordered_map<uint8, bool> uint8boolu_map;
typedef std::unordered_map<uint8, nsstring> uint8stringu_map;
typedef std::unordered_map<uint16, char> uint16charu_map;
typedef std::unordered_map<uint16, char16> uint16char16u_map;
typedef std::unordered_map<uint16, char32> uint16char32u_map;
typedef std::unordered_map<uint16, wchar> uint16wcharu_map;
typedef std::unordered_map<uint16, int16> uint16sintu_map;
typedef std::unordered_map<uint16, int32> uint16intu_map;
typedef std::unordered_map<uint16, int32> uint16lintu_map;
typedef std::unordered_map<uint16, int64> uint16llintu_map;
typedef std::unordered_map<uint16, uint8> uint16ucharu_map;
typedef std::unordered_map<uint16, uint16> uint16usintu_map;
typedef std::unordered_map<uint16, uint32> uint16uintu_map;
typedef std::unordered_map<uint16, uint32> uint16ulintu_map;
typedef std::unordered_map<uint16, uint64> uint16ullintu_map;
typedef std::unordered_map<uint16, float> uint16floatu_map;
typedef std::unordered_map<uint16, double> uint16doubleu_map;
typedef std::unordered_map<uint16, ldouble> uint16ldoubleu_map;
typedef std::unordered_map<uint16, bool> uint16boolu_map;
typedef std::unordered_map<uint16, nsstring> uint16stringu_map;
typedef std::unordered_map<uint32, char> uint32charu_map;
typedef std::unordered_map<uint32, char16> uint32char16u_map;
typedef std::unordered_map<uint32, char32> uint32char32u_map;
typedef std::unordered_map<uint32, wchar> uint32wcharu_map;
typedef std::unordered_map<uint32, int16> uint32sintu_map;
typedef std::unordered_map<uint32, int32> uint32intu_map;
typedef std::unordered_map<uint32, int32> uint32lintu_map;
typedef std::unordered_map<uint32, int64> uint32llintu_map;
typedef std::unordered_map<uint32, uint8> uint32ucharu_map;
typedef std::unordered_map<uint32, uint16> uint32usintu_map;
typedef std::unordered_map<uint32, uint32> uint32uintu_map;
typedef std::unordered_map<uint32, uint32> uint32ulintu_map;
typedef std::unordered_map<uint32, uint64> uint32ullintu_map;
typedef std::unordered_map<uint32, float> uint32floatu_map;
typedef std::unordered_map<uint32, double> uint32doubleu_map;
typedef std::unordered_map<uint32, ldouble> uint32ldoubleu_map;
typedef std::unordered_map<uint32, bool> uint32boolu_map;
typedef std::unordered_map<uint32, nsstring> uint32stringu_map;
typedef std::unordered_map<uint32, char> uint32charu_map;
typedef std::unordered_map<uint32, char16> uint32char16u_map;
typedef std::unordered_map<uint32, char32> uint32char32u_map;
typedef std::unordered_map<uint32, wchar> uint32wcharu_map;
typedef std::unordered_map<uint32, int16> uint32sintu_map;
typedef std::unordered_map<uint32, int32> uint32intu_map;
typedef std::unordered_map<uint32, int32> uint32lintu_map;
typedef std::unordered_map<uint32, int64> uint32llintu_map;
typedef std::unordered_map<uint32, uint8> uint32ucharu_map;
typedef std::unordered_map<uint32, uint16> uint32usintu_map;
typedef std::unordered_map<uint32, uint32> uint32uintu_map;
typedef std::unordered_map<uint32, uint32> uint32ulintu_map;
typedef std::unordered_map<uint32, uint64> uint32ullintu_map;
typedef std::unordered_map<uint32, float> uint32floatu_map;
typedef std::unordered_map<uint32, double> uint32doubleu_map;
typedef std::unordered_map<uint32, ldouble> uint32ldoubleu_map;
typedef std::unordered_map<uint32, bool> uint32boolu_map;
typedef std::unordered_map<uint32, nsstring> uint32stringu_map;
typedef std::unordered_map<uint64, char> uint64charu_map;
typedef std::unordered_map<uint64, char16> uint64char16u_map;
typedef std::unordered_map<uint64, char32> uint64char32u_map;
typedef std::unordered_map<uint64, wchar> uint64wcharu_map;
typedef std::unordered_map<uint64, int16> uint64sintu_map;
typedef std::unordered_map<uint64, int32> uint64intu_map;
typedef std::unordered_map<uint64, int32> uint64lintu_map;
typedef std::unordered_map<uint64, int64> uint64llintu_map;
typedef std::unordered_map<uint64, uint8> uint64ucharu_map;
typedef std::unordered_map<uint64, uint16> uint64usintu_map;
typedef std::unordered_map<uint64, uint32> uint64uintu_map;
typedef std::unordered_map<uint64, uint32> uint64ulintu_map;
typedef std::unordered_map<uint64, uint64> uint64ullintu_map;
typedef std::unordered_map<uint64, float> uint64floatu_map;
typedef std::unordered_map<uint64, double> uint64doubleu_map;
typedef std::unordered_map<uint64, ldouble> uint64ldoubleu_map;
typedef std::unordered_map<uint64, bool> uint64boolu_map;
typedef std::unordered_map<uint64, nsstring> uint64stringu_map;
typedef std::unordered_map<float, char> floatcharu_map;
typedef std::unordered_map<float, char16> floatchar16u_map;
typedef std::unordered_map<float, char32> floatchar32u_map;
typedef std::unordered_map<float, wchar> floatwcharu_map;
typedef std::unordered_map<float, int16> floatsintu_map;
typedef std::unordered_map<float, int32> floatintu_map;
typedef std::unordered_map<float, int32> floatlintu_map;
typedef std::unordered_map<float, int64> floatllintu_map;
typedef std::unordered_map<float, uint8> floatucharu_map;
typedef std::unordered_map<float, uint16> floatusintu_map;
typedef std::unordered_map<float, uint32> floatuintu_map;
typedef std::unordered_map<float, uint32> floatulintu_map;
typedef std::unordered_map<float, uint64> floatullintu_map;
typedef std::unordered_map<float, float> floatfloatu_map;
typedef std::unordered_map<float, double> floatdoubleu_map;
typedef std::unordered_map<float, ldouble> floatldoubleu_map;
typedef std::unordered_map<float, bool> floatboolu_map;
typedef std::unordered_map<float, nsstring> floatstringu_map;
typedef std::unordered_map<double, char> doublecharu_map;
typedef std::unordered_map<double, char16> doublechar16u_map;
typedef std::unordered_map<double, char32> doublechar32u_map;
typedef std::unordered_map<double, wchar> doublewcharu_map;
typedef std::unordered_map<double, int16> doublesintu_map;
typedef std::unordered_map<double, int32> doubleintu_map;
typedef std::unordered_map<double, int32> doublelintu_map;
typedef std::unordered_map<double, int64> doublellintu_map;
typedef std::unordered_map<double, uint8> doubleucharu_map;
typedef std::unordered_map<double, uint16> doubleusintu_map;
typedef std::unordered_map<double, uint32> doubleuintu_map;
typedef std::unordered_map<double, uint32> doubleulintu_map;
typedef std::unordered_map<double, uint64> doubleullintu_map;
typedef std::unordered_map<double, float> doublefloatu_map;
typedef std::unordered_map<double, double> doubledoubleu_map;
typedef std::unordered_map<double, ldouble> doubleldoubleu_map;
typedef std::unordered_map<double, bool> doubleboolu_map;
typedef std::unordered_map<double, nsstring> doublestringu_map;
typedef std::unordered_map<ldouble, char> ldoublecharu_map;
typedef std::unordered_map<ldouble, char16> ldoublechar16u_map;
typedef std::unordered_map<ldouble, char32> ldoublechar32u_map;
typedef std::unordered_map<ldouble, wchar> ldoublewcharu_map;
typedef std::unordered_map<ldouble, int16> ldoublesintu_map;
typedef std::unordered_map<ldouble, int32> ldoubleintu_map;
typedef std::unordered_map<ldouble, int32> ldoublelintu_map;
typedef std::unordered_map<ldouble, int64> ldoublellintu_map;
typedef std::unordered_map<ldouble, uint8> ldoubleucharu_map;
typedef std::unordered_map<ldouble, uint16> ldoubleusintu_map;
typedef std::unordered_map<ldouble, uint32> ldoubleuintu_map;
typedef std::unordered_map<ldouble, uint32> ldoubleulintu_map;
typedef std::unordered_map<ldouble, uint64> ldoubleullintu_map;
typedef std::unordered_map<ldouble, float> ldoublefloatu_map;
typedef std::unordered_map<ldouble, double> ldoubledoubleu_map;
typedef std::unordered_map<ldouble, ldouble> ldoubleldoubleu_map;
typedef std::unordered_map<ldouble, bool> ldoubleboolu_map;
typedef std::unordered_map<ldouble, nsstring> ldoublestringu_map;
typedef std::unordered_map<bool, char> boolcharu_map;
typedef std::unordered_map<bool, char16> boolchar16u_map;
typedef std::unordered_map<bool, char32> boolchar32u_map;
typedef std::unordered_map<bool, wchar> boolwcharu_map;
typedef std::unordered_map<bool, int16> boolsintu_map;
typedef std::unordered_map<bool, int32> boolintu_map;
typedef std::unordered_map<bool, int32> boollintu_map;
typedef std::unordered_map<bool, int64> boolllintu_map;
typedef std::unordered_map<bool, uint8> boolucharu_map;
typedef std::unordered_map<bool, uint16> boolusintu_map;
typedef std::unordered_map<bool, uint32> booluintu_map;
typedef std::unordered_map<bool, uint32> boolulintu_map;
typedef std::unordered_map<bool, uint64> boolullintu_map;
typedef std::unordered_map<bool, float> boolfloatu_map;
typedef std::unordered_map<bool, double> booldoubleu_map;
typedef std::unordered_map<bool, ldouble> boolldoubleu_map;
typedef std::unordered_map<bool, bool> boolboolu_map;
typedef std::unordered_map<bool, nsstring> boolstringu_map;
typedef std::unordered_map<nsstring, char> nsstringcharu_map;
typedef std::unordered_map<nsstring, char16> nsstringchar16u_map;
typedef std::unordered_map<nsstring, char32> nsstringchar32u_map;
typedef std::unordered_map<nsstring, wchar> nsstringwcharu_map;
typedef std::unordered_map<nsstring, int16> nsstringsintu_map;
typedef std::unordered_map<nsstring, int32> nsstringintu_map;
typedef std::unordered_map<nsstring, int32> nsstringlintu_map;
typedef std::unordered_map<nsstring, int64> nsstringllintu_map;
typedef std::unordered_map<nsstring, uint8> nsstringucharu_map;
typedef std::unordered_map<nsstring, uint16> nsstringusintu_map;
typedef std::unordered_map<nsstring, uint32> nsstringuintu_map;
typedef std::unordered_map<nsstring, uint32> nsstringulintu_map;
typedef std::unordered_map<nsstring, uint64> nsstringullintu_map;
typedef std::unordered_map<nsstring, float> nsstringfloatu_map;
typedef std::unordered_map<nsstring, double> nsstringdoubleu_map;
typedef std::unordered_map<nsstring, ldouble> nsstringldoubleu_map;
typedef std::unordered_map<nsstring, bool> nsstringboolu_map;
typedef std::unordered_map<nsstring, nsstring> nsstringstringu_map;

typedef std::multimap<char, char> nscharcharmultimap;
typedef std::multimap<char, char16> nscharchar16multimap;
typedef std::multimap<char, char32> nscharchar32multimap;
typedef std::multimap<char, wchar> nscharwcharmultimap;
typedef std::multimap<char, int16> nscharsintmultimap;
typedef std::multimap<char, int32> nscharintmultimap;
typedef std::multimap<char, int32> nscharlintmultimap;
typedef std::multimap<char, int64> nscharllintmultimap;
typedef std::multimap<char, uint8> nscharucharmultimap;
typedef std::multimap<char, uint16> nscharusintmultimap;
typedef std::multimap<char, uint32> nscharuintmultimap;
typedef std::multimap<char, uint32> nscharulintmultimap;
typedef std::multimap<char, uint64> nscharullintmultimap;
typedef std::multimap<char, float> nscharfloatmultimap;
typedef std::multimap<char, double> nschardoublemultimap;
typedef std::multimap<char, ldouble> nscharldoublemultimap;
typedef std::multimap<char, bool> nscharboolmultimap;
typedef std::multimap<char, nsstring> nscharstringmultimap;
typedef std::multimap<char16, char> char16charmultimap;
typedef std::multimap<char16, char16> char16char16multimap;
typedef std::multimap<char16, char32> char16char32multimap;
typedef std::multimap<char16, wchar> char16wcharmultimap;
typedef std::multimap<char16, int16> char16sintmultimap;
typedef std::multimap<char16, int32> char16intmultimap;
typedef std::multimap<char16, int32> char16lintmultimap;
typedef std::multimap<char16, int64> char16llintmultimap;
typedef std::multimap<char16, uint8> char16ucharmultimap;
typedef std::multimap<char16, uint16> char16usintmultimap;
typedef std::multimap<char16, uint32> char16uintmultimap;
typedef std::multimap<char16, uint32> char16ulintmultimap;
typedef std::multimap<char16, uint64> char16ullintmultimap;
typedef std::multimap<char16, float> char16floatmultimap;
typedef std::multimap<char16, double> char16doublemultimap;
typedef std::multimap<char16, ldouble> char16ldoublemultimap;
typedef std::multimap<char16, bool> char16boolmultimap;
typedef std::multimap<char16, nsstring> char16stringmultimap;
typedef std::multimap<char32, char> char32charmultimap;
typedef std::multimap<char32, char16> char32char16multimap;
typedef std::multimap<char32, char32> char32char32multimap;
typedef std::multimap<char32, wchar> char32wcharmultimap;
typedef std::multimap<char32, int16> char32sintmultimap;
typedef std::multimap<char32, int32> char32intmultimap;
typedef std::multimap<char32, int32> char32lintmultimap;
typedef std::multimap<char32, int64> char32llintmultimap;
typedef std::multimap<char32, uint8> char32ucharmultimap;
typedef std::multimap<char32, uint16> char32usintmultimap;
typedef std::multimap<char32, uint32> char32uintmultimap;
typedef std::multimap<char32, uint32> char32ulintmultimap;
typedef std::multimap<char32, uint64> char32ullintmultimap;
typedef std::multimap<char32, float> char32floatmultimap;
typedef std::multimap<char32, double> char32doublemultimap;
typedef std::multimap<char32, ldouble> char32ldoublemultimap;
typedef std::multimap<char32, bool> char32boolmultimap;
typedef std::multimap<char32, nsstring> char32stringmultimap;
typedef std::multimap<wchar, char> wcharcharmultimap;
typedef std::multimap<wchar, char16> wcharchar16multimap;
typedef std::multimap<wchar, char32> wcharchar32multimap;
typedef std::multimap<wchar, wchar> wcharwcharmultimap;
typedef std::multimap<wchar, int16> wcharsintmultimap;
typedef std::multimap<wchar, int32> wcharintmultimap;
typedef std::multimap<wchar, int32> wcharlintmultimap;
typedef std::multimap<wchar, int64> wcharllintmultimap;
typedef std::multimap<wchar, uint8> wcharucharmultimap;
typedef std::multimap<wchar, uint16> wcharusintmultimap;
typedef std::multimap<wchar, uint32> wcharuintmultimap;
typedef std::multimap<wchar, uint32> wcharulintmultimap;
typedef std::multimap<wchar, uint64> wcharullintmultimap;
typedef std::multimap<wchar, float> wcharfloatmultimap;
typedef std::multimap<wchar, double> wchardoublemultimap;
typedef std::multimap<wchar, ldouble> wcharldoublemultimap;
typedef std::multimap<wchar, bool> wcharboolmultimap;
typedef std::multimap<wchar, nsstring> wcharstringmultimap;
typedef std::multimap<int16, char> int16charmultimap;
typedef std::multimap<int16, char16> int16char16multimap;
typedef std::multimap<int16, char32> int16char32multimap;
typedef std::multimap<int16, wchar> int16wcharmultimap;
typedef std::multimap<int16, int16> int16sintmultimap;
typedef std::multimap<int16, int32> int16intmultimap;
typedef std::multimap<int16, int32> int16lintmultimap;
typedef std::multimap<int16, int64> int16llintmultimap;
typedef std::multimap<int16, uint8> int16ucharmultimap;
typedef std::multimap<int16, uint16> int16usintmultimap;
typedef std::multimap<int16, uint32> int16uintmultimap;
typedef std::multimap<int16, uint32> int16ulintmultimap;
typedef std::multimap<int16, uint64> int16ullintmultimap;
typedef std::multimap<int16, float> int16floatmultimap;
typedef std::multimap<int16, double> int16doublemultimap;
typedef std::multimap<int16, ldouble> int16ldoublemultimap;
typedef std::multimap<int16, bool> int16boolmultimap;
typedef std::multimap<int16, nsstring> int16stringmultimap;
typedef std::multimap<int32, char> int32charmultimap;
typedef std::multimap<int32, char16> int32char16multimap;
typedef std::multimap<int32, char32> int32char32multimap;
typedef std::multimap<int32, wchar> int32wcharmultimap;
typedef std::multimap<int32, int16> int32sintmultimap;
typedef std::multimap<int32, int32> int32intmultimap;
typedef std::multimap<int32, int32> int32lintmultimap;
typedef std::multimap<int32, int64> int32llintmultimap;
typedef std::multimap<int32, uint8> int32ucharmultimap;
typedef std::multimap<int32, uint16> int32usintmultimap;
typedef std::multimap<int32, uint32> int32uintmultimap;
typedef std::multimap<int32, uint32> int32ulintmultimap;
typedef std::multimap<int32, uint64> int32ullintmultimap;
typedef std::multimap<int32, float> int32floatmultimap;
typedef std::multimap<int32, double> int32doublemultimap;
typedef std::multimap<int32, ldouble> int32ldoublemultimap;
typedef std::multimap<int32, bool> int32boolmultimap;
typedef std::multimap<int32, nsstring> int32stringmultimap;
typedef std::multimap<int32, char> int32charmultimap;
typedef std::multimap<int32, char16> int32char16multimap;
typedef std::multimap<int32, char32> int32char32multimap;
typedef std::multimap<int32, wchar> int32wcharmultimap;
typedef std::multimap<int32, int16> int32sintmultimap;
typedef std::multimap<int32, int32> int32intmultimap;
typedef std::multimap<int32, int32> int32lintmultimap;
typedef std::multimap<int32, int64> int32llintmultimap;
typedef std::multimap<int32, uint8> int32ucharmultimap;
typedef std::multimap<int32, uint16> int32usintmultimap;
typedef std::multimap<int32, uint32> int32uintmultimap;
typedef std::multimap<int32, uint32> int32ulintmultimap;
typedef std::multimap<int32, uint64> int32ullintmultimap;
typedef std::multimap<int32, float> int32floatmultimap;
typedef std::multimap<int32, double> int32doublemultimap;
typedef std::multimap<int32, ldouble> int32ldoublemultimap;
typedef std::multimap<int32, bool> int32boolmultimap;
typedef std::multimap<int32, nsstring> int32stringmultimap;
typedef std::multimap<int64, char> int64charmultimap;
typedef std::multimap<int64, char16> int64char16multimap;
typedef std::multimap<int64, char32> int64char32multimap;
typedef std::multimap<int64, wchar> int64wcharmultimap;
typedef std::multimap<int64, int16> int64sintmultimap;
typedef std::multimap<int64, int32> int64intmultimap;
typedef std::multimap<int64, int32> int64lintmultimap;
typedef std::multimap<int64, int64> int64llintmultimap;
typedef std::multimap<int64, uint8> int64ucharmultimap;
typedef std::multimap<int64, uint16> int64usintmultimap;
typedef std::multimap<int64, uint32> int64uintmultimap;
typedef std::multimap<int64, uint32> int64ulintmultimap;
typedef std::multimap<int64, uint64> int64ullintmultimap;
typedef std::multimap<int64, float> int64floatmultimap;
typedef std::multimap<int64, double> int64doublemultimap;
typedef std::multimap<int64, ldouble> int64ldoublemultimap;
typedef std::multimap<int64, bool> int64boolmultimap;
typedef std::multimap<int64, nsstring> int64stringmultimap;
typedef std::multimap<uint8, char> uint8charmultimap;
typedef std::multimap<uint8, char16> uint8char16multimap;
typedef std::multimap<uint8, char32> uint8char32multimap;
typedef std::multimap<uint8, wchar> uint8wcharmultimap;
typedef std::multimap<uint8, int16> uint8sintmultimap;
typedef std::multimap<uint8, int32> uint8intmultimap;
typedef std::multimap<uint8, int32> uint8lintmultimap;
typedef std::multimap<uint8, int64> uint8llintmultimap;
typedef std::multimap<uint8, uint8> uint8ucharmultimap;
typedef std::multimap<uint8, uint16> uint8usintmultimap;
typedef std::multimap<uint8, uint32> uint8uintmultimap;
typedef std::multimap<uint8, uint32> uint8ulintmultimap;
typedef std::multimap<uint8, uint64> uint8ullintmultimap;
typedef std::multimap<uint8, float> uint8floatmultimap;
typedef std::multimap<uint8, double> uint8doublemultimap;
typedef std::multimap<uint8, ldouble> uint8ldoublemultimap;
typedef std::multimap<uint8, bool> uint8boolmultimap;
typedef std::multimap<uint8, nsstring> uint8stringmultimap;
typedef std::multimap<uint16, char> uint16charmultimap;
typedef std::multimap<uint16, char16> uint16char16multimap;
typedef std::multimap<uint16, char32> uint16char32multimap;
typedef std::multimap<uint16, wchar> uint16wcharmultimap;
typedef std::multimap<uint16, int16> uint16sintmultimap;
typedef std::multimap<uint16, int32> uint16intmultimap;
typedef std::multimap<uint16, int32> uint16lintmultimap;
typedef std::multimap<uint16, int64> uint16llintmultimap;
typedef std::multimap<uint16, uint8> uint16ucharmultimap;
typedef std::multimap<uint16, uint16> uint16usintmultimap;
typedef std::multimap<uint16, uint32> uint16uintmultimap;
typedef std::multimap<uint16, uint32> uint16ulintmultimap;
typedef std::multimap<uint16, uint64> uint16ullintmultimap;
typedef std::multimap<uint16, float> uint16floatmultimap;
typedef std::multimap<uint16, double> uint16doublemultimap;
typedef std::multimap<uint16, ldouble> uint16ldoublemultimap;
typedef std::multimap<uint16, bool> uint16boolmultimap;
typedef std::multimap<uint16, nsstring> uint16stringmultimap;
typedef std::multimap<uint32, char> uint32charmultimap;
typedef std::multimap<uint32, char16> uint32char16multimap;
typedef std::multimap<uint32, char32> uint32char32multimap;
typedef std::multimap<uint32, wchar> uint32wcharmultimap;
typedef std::multimap<uint32, int16> uint32sintmultimap;
typedef std::multimap<uint32, int32> uint32intmultimap;
typedef std::multimap<uint32, int32> uint32lintmultimap;
typedef std::multimap<uint32, int64> uint32llintmultimap;
typedef std::multimap<uint32, uint8> uint32ucharmultimap;
typedef std::multimap<uint32, uint16> uint32usintmultimap;
typedef std::multimap<uint32, uint32> uint32uintmultimap;
typedef std::multimap<uint32, uint32> uint32ulintmultimap;
typedef std::multimap<uint32, uint64> uint32ullintmultimap;
typedef std::multimap<uint32, float> uint32floatmultimap;
typedef std::multimap<uint32, double> uint32doublemultimap;
typedef std::multimap<uint32, ldouble> uint32ldoublemultimap;
typedef std::multimap<uint32, bool> uint32boolmultimap;
typedef std::multimap<uint32, nsstring> uint32stringmultimap;
typedef std::multimap<uint32, char> uint32charmultimap;
typedef std::multimap<uint32, char16> uint32char16multimap;
typedef std::multimap<uint32, char32> uint32char32multimap;
typedef std::multimap<uint32, wchar> uint32wcharmultimap;
typedef std::multimap<uint32, int16> uint32sintmultimap;
typedef std::multimap<uint32, int32> uint32intmultimap;
typedef std::multimap<uint32, int32> uint32lintmultimap;
typedef std::multimap<uint32, int64> uint32llintmultimap;
typedef std::multimap<uint32, uint8> uint32ucharmultimap;
typedef std::multimap<uint32, uint16> uint32usintmultimap;
typedef std::multimap<uint32, uint32> uint32uintmultimap;
typedef std::multimap<uint32, uint32> uint32ulintmultimap;
typedef std::multimap<uint32, uint64> uint32ullintmultimap;
typedef std::multimap<uint32, float> uint32floatmultimap;
typedef std::multimap<uint32, double> uint32doublemultimap;
typedef std::multimap<uint32, ldouble> uint32ldoublemultimap;
typedef std::multimap<uint32, bool> uint32boolmultimap;
typedef std::multimap<uint32, nsstring> uint32stringmultimap;
typedef std::multimap<uint64, char> uint64charmultimap;
typedef std::multimap<uint64, char16> uint64char16multimap;
typedef std::multimap<uint64, char32> uint64char32multimap;
typedef std::multimap<uint64, wchar> uint64wcharmultimap;
typedef std::multimap<uint64, int16> uint64sintmultimap;
typedef std::multimap<uint64, int32> uint64intmultimap;
typedef std::multimap<uint64, int32> uint64lintmultimap;
typedef std::multimap<uint64, int64> uint64llintmultimap;
typedef std::multimap<uint64, uint8> uint64ucharmultimap;
typedef std::multimap<uint64, uint16> uint64usintmultimap;
typedef std::multimap<uint64, uint32> uint64uintmultimap;
typedef std::multimap<uint64, uint32> uint64ulintmultimap;
typedef std::multimap<uint64, uint64> uint64ullintmultimap;
typedef std::multimap<uint64, float> uint64floatmultimap;
typedef std::multimap<uint64, double> uint64doublemultimap;
typedef std::multimap<uint64, ldouble> uint64ldoublemultimap;
typedef std::multimap<uint64, bool> uint64boolmultimap;
typedef std::multimap<uint64, nsstring> uint64stringmultimap;
typedef std::multimap<float, char> floatcharmultimap;
typedef std::multimap<float, char16> floatchar16multimap;
typedef std::multimap<float, char32> floatchar32multimap;
typedef std::multimap<float, wchar> floatwcharmultimap;
typedef std::multimap<float, int16> floatsintmultimap;
typedef std::multimap<float, int32> floatintmultimap;
typedef std::multimap<float, int32> floatlintmultimap;
typedef std::multimap<float, int64> floatllintmultimap;
typedef std::multimap<float, uint8> floatucharmultimap;
typedef std::multimap<float, uint16> floatusintmultimap;
typedef std::multimap<float, uint32> floatuintmultimap;
typedef std::multimap<float, uint32> floatulintmultimap;
typedef std::multimap<float, uint64> floatullintmultimap;
typedef std::multimap<float, float> floatfloatmultimap;
typedef std::multimap<float, double> floatdoublemultimap;
typedef std::multimap<float, ldouble> floatldoublemultimap;
typedef std::multimap<float, bool> floatboolmultimap;
typedef std::multimap<float, nsstring> floatstringmultimap;
typedef std::multimap<double, char> doublecharmultimap;
typedef std::multimap<double, char16> doublechar16multimap;
typedef std::multimap<double, char32> doublechar32multimap;
typedef std::multimap<double, wchar> doublewcharmultimap;
typedef std::multimap<double, int16> doublesintmultimap;
typedef std::multimap<double, int32> doubleintmultimap;
typedef std::multimap<double, int32> doublelintmultimap;
typedef std::multimap<double, int64> doublellintmultimap;
typedef std::multimap<double, uint8> doubleucharmultimap;
typedef std::multimap<double, uint16> doubleusintmultimap;
typedef std::multimap<double, uint32> doubleuintmultimap;
typedef std::multimap<double, uint32> doubleulintmultimap;
typedef std::multimap<double, uint64> doubleullintmultimap;
typedef std::multimap<double, float> doublefloatmultimap;
typedef std::multimap<double, double> doubledoublemultimap;
typedef std::multimap<double, ldouble> doubleldoublemultimap;
typedef std::multimap<double, bool> doubleboolmultimap;
typedef std::multimap<double, nsstring> doublestringmultimap;
typedef std::multimap<ldouble, char> ldoublecharmultimap;
typedef std::multimap<ldouble, char16> ldoublechar16multimap;
typedef std::multimap<ldouble, char32> ldoublechar32multimap;
typedef std::multimap<ldouble, wchar> ldoublewcharmultimap;
typedef std::multimap<ldouble, int16> ldoublesintmultimap;
typedef std::multimap<ldouble, int32> ldoubleintmultimap;
typedef std::multimap<ldouble, int32> ldoublelintmultimap;
typedef std::multimap<ldouble, int64> ldoublellintmultimap;
typedef std::multimap<ldouble, uint8> ldoubleucharmultimap;
typedef std::multimap<ldouble, uint16> ldoubleusintmultimap;
typedef std::multimap<ldouble, uint32> ldoubleuintmultimap;
typedef std::multimap<ldouble, uint32> ldoubleulintmultimap;
typedef std::multimap<ldouble, uint64> ldoubleullintmultimap;
typedef std::multimap<ldouble, float> ldoublefloatmultimap;
typedef std::multimap<ldouble, double> ldoubledoublemultimap;
typedef std::multimap<ldouble, ldouble> ldoubleldoublemultimap;
typedef std::multimap<ldouble, bool> ldoubleboolmultimap;
typedef std::multimap<ldouble, nsstring> ldoublestringmultimap;
typedef std::multimap<bool, char> boolcharmultimap;
typedef std::multimap<bool, char16> boolchar16multimap;
typedef std::multimap<bool, char32> boolchar32multimap;
typedef std::multimap<bool, wchar> boolwcharmultimap;
typedef std::multimap<bool, int16> boolsintmultimap;
typedef std::multimap<bool, int32> boolintmultimap;
typedef std::multimap<bool, int32> boollintmultimap;
typedef std::multimap<bool, int64> boolllintmultimap;
typedef std::multimap<bool, uint8> boolucharmultimap;
typedef std::multimap<bool, uint16> boolusintmultimap;
typedef std::multimap<bool, uint32> booluintmultimap;
typedef std::multimap<bool, uint32> boolulintmultimap;
typedef std::multimap<bool, uint64> boolullintmultimap;
typedef std::multimap<bool, float> boolfloatmultimap;
typedef std::multimap<bool, double> booldoublemultimap;
typedef std::multimap<bool, ldouble> boolldoublemultimap;
typedef std::multimap<bool, bool> boolboolmultimap;
typedef std::multimap<bool, nsstring> boolstringmultimap;
typedef std::multimap<nsstring, char> nsstringcharmultimap;
typedef std::multimap<nsstring, char16> nsstringchar16multimap;
typedef std::multimap<nsstring, char32> nsstringchar32multimap;
typedef std::multimap<nsstring, wchar> nsstringwcharmultimap;
typedef std::multimap<nsstring, int16> nsstringsintmultimap;
typedef std::multimap<nsstring, int32> nsstringintmultimap;
typedef std::multimap<nsstring, int32> nsstringlintmultimap;
typedef std::multimap<nsstring, int64> nsstringllintmultimap;
typedef std::multimap<nsstring, uint8> nsstringucharmultimap;
typedef std::multimap<nsstring, uint16> nsstringusintmultimap;
typedef std::multimap<nsstring, uint32> nsstringuintmultimap;
typedef std::multimap<nsstring, uint32> nsstringulintmultimap;
typedef std::multimap<nsstring, uint64> nsstringullintmultimap;
typedef std::multimap<nsstring, float> nsstringfloatmultimap;
typedef std::multimap<nsstring, double> nsstringdoublemultimap;
typedef std::multimap<nsstring, ldouble> nsstringldoublemultimap;
typedef std::multimap<nsstring, bool> nsstringboolmultimap;
typedef std::multimap<nsstring, nsstring> nsstringstringmultimap;

class nsmaterial;
class nsentity;

typedef std::set<nsmaterial*> nspmatset;
typedef std::map<uint32, nsentity*> uint32pentity_map;
typedef std::set<nsentity*> nspentityset;

GLEWContext * glewGetContext();
bool GLError(nsstring errorMessage);

#endif
