#include <ScriptObject.h>
#include <Territory/InstanceContent.h>

using namespace Sapphire;

class BattleintheBigKeep : public Sapphire::ScriptAPI::InstanceContentScript
{
public:
  BattleintheBigKeep() : Sapphire::ScriptAPI::InstanceContentScript( 20030 )
  { }

  void onInit( InstanceContent& instance ) override
  {
    instance.addEObj( "sgbg_f1d3_a1_gar2", 2000551, 4277027, 4277030, 4, { -3.577190f, 48.000000f, -12.625050f }, 1.000000f, 0.000000f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_1", 2000552, 4277005, 4277031, 4, { 21.455280f, 48.000011f, -12.604770f }, 1.000000f, 0.000000f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_2", 2000553, 4277029, 4277032, 4, { -3.963791f, 48.000000f, 9.699657f }, 1.000000f, 0.000000f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_3", 2000554, 4277028, 4277033, 4, { 21.081841f, 47.999920f, 9.731779f }, 1.000000f, 0.000000f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "Entrance", 2000491, 4246565, 4247629, 5, { -254.388702f, -8.857766f, -0.323940f }, 1.000000f, 0.000000f, 0); 
    // States -> vf_lock_on (id: 11) vf_lock_of (id: 12) 
    instance.addEObj( "unknown_0", 2000492, 0, 4247690, 4, { -266.179810f, -9.200000f, -0.162207f }, 1.000000f, 0.000000f, 0); 
    instance.addEObj( "Exit", 2000493, 4246574, 4247691, 4, { -12.466670f, 48.285069f, -0.015320f }, 0.991760f, 0.000048f, 0); 
    // States -> vf_htras1_on (id: 2) vf_htras1_of (id: 3) 
    instance.addEObj( "sgvf_w_lvd_b0094", 2000494, 4246576, 4247817, 4, { -81.527344f, 0.000003f, -1.602537f }, 1.000000f, 0.000000f, 0); 
    // States -> vf_line_on (id: 12) vf_line_of (id: 13) 
    instance.addEObj( "sgvf_w_lvd_b0118", 2000495, 4246594, 4247819, 4, { -81.412201f, 0.000003f, 0.186550f }, 1.000000f, 0.000000f, 0); 
    // States -> vf_bextwall_on (id: 3) vf_bextwall_of (id: 4) 
    instance.addEObj( "sgvf_w_lvd_b0094_1", 2000496, 4246595, 4247821, 4, { 199.816895f, 21.957701f, -104.781303f }, 0.991760f, 0.000048f, 0); 
    // States -> vf_line_on (id: 12) vf_line_of (id: 13) 
    instance.addEObj( "sgpl_w_lvd_b0118_f1d2", 2000497, 4246596, 4247825, 4, { 200.915497f, 21.957701f, -104.598198f }, 0.991760f, 0.000048f, 0); 
    // States -> vf_bextwall_on (id: 3) vf_bextwall_of (id: 4) 
    instance.addEObj( "sgvf_w_lvd_b0249", 2000498, 4246617, 4247827, 4, { 34.775269f, 48.081181f, 0.381470f }, 0.991760f, 0.000048f, 0); 
    // States -> vf_line_on (id: 10) vf_line_of (id: 11) 
    instance.addEObj( "sgvf_w_lvd_b0250", 2000499, 4246618, 4247828, 4, { 34.775269f, 48.081181f, -0.839294f }, 0.991760f, 0.000048f, 0); 
    // States -> vf_bextwall_on (id: 3) vf_bextwall_of (id: 4) 
    instance.addEObj( "Shortcut", 2000700, 0, 4247830, 4, { -246.833496f, -8.499996f, 4.016408f }, 1.000000f, 0.000000f, 0); 
    instance.addEObj( "Aetherialflow", 2000500, 4247835, 4247836, 4, { -10.236110f, -0.000000f, -0.015320f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "PresenceChamberdoor", 2000502, 4308888, 4247846, 4, { 60.483269f, 48.174511f, -0.015320f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "BloodyTranseptdoor", 2000687, 4308274, 4309587, 4, { 199.985397f, 22.200001f, -103.389503f }, 1.000000f, 0.000000f, 0); 
    instance.addEObj( "sgvf_f1d1_b0008", 2002741, 4388278, 4388279, 4, { -82.462273f, 0.000002f, 0.045686f }, 1.000000f, 0.000000f, 0); 
    // States -> vf_baria1_off (id: 3) vf_baria_on (id: 6) 
    instance.addEObj( "sgbg_f1d3_a1_gar1", 2000509, 4271520, 4274705, 4, { 134.111496f, 22.079769f, -5.935791f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_1", 2000510, 4271522, 4274706, 4, { 142.229202f, 22.079769f, -6.088379f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_2", 2000511, 4271529, 4274707, 4, { 158.129196f, 22.079769f, -6.088379f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_3", 2000512, 4271531, 4274708, 4, { 133.958893f, 22.079769f, 6.271362f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_4", 2000513, 4271532, 4274709, 4, { 142.015594f, 22.079769f, 6.271362f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_5", 2000514, 4271533, 4274710, 4, { 158.190201f, 22.079769f, 6.271362f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_6", 2000521, 4274657, 4274711, 4, { 193.987900f, 22.110291f, 42.099609f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_7", 2000522, 4274656, 4274712, 4, { 193.804794f, 22.079769f, 57.938480f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_8", 2000523, 4274655, 4274713, 4, { 193.804794f, 22.079769f, 65.964722f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_9", 2000524, 4274661, 4274714, 4, { 206.317093f, 22.079769f, 42.038570f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_10", 2000525, 4274660, 4274715, 4, { 206.286697f, 22.079769f, 57.968990f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_11", 2000526, 4274659, 4274716, 4, { 206.286697f, 22.079769f, 65.995239f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_12", 2000533, 4274648, 4274717, 4, { 241.992798f, 22.079769f, -6.332520f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_13", 2000534, 4274649, 4274718, 4, { 258.075806f, 22.079769f, -6.332520f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_14", 2000535, 4274650, 4274719, 4, { 265.949493f, 22.079769f, -6.271484f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_15", 2000536, 4274651, 4274720, 4, { 241.931793f, 22.079769f, 6.240906f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_16", 2000537, 4274652, 4274721, 4, { 258.075806f, 22.079769f, 6.271362f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_17", 2000538, 4274653, 4274722, 4, { 266.071503f, 22.079769f, 6.271362f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_18", 2000545, 4274662, 4274724, 4, { 193.804794f, 22.079769f, -34.042912f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_19", 2000546, 4274667, 4274725, 4, { 193.804794f, 22.079769f, -42.008121f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_20", 2000547, 4274666, 4274726, 4, { 193.804794f, 22.079769f, -58.335270f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_21", 2000548, 4274663, 4274728, 4, { 206.317093f, 22.079769f, -33.890320f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_22", 2000549, 4274665, 4274729, 4, { 206.317093f, 22.079769f, -41.916561f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar1_23", 2000550, 4274664, 4274730, 4, { 206.317093f, 22.079769f, -58.274231f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_f1d3_a1_gar2_4", 2000503, 4274671, 4274731, 4, { 134.111496f, 22.079769f, -5.935791f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_5", 2000504, 4274672, 4274732, 4, { 142.229202f, 22.079769f, -6.088379f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_6", 2000505, 4274673, 4274733, 4, { 158.129196f, 22.079769f, -6.088379f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_7", 2000506, 4274674, 4274734, 4, { 133.958893f, 22.079769f, 6.271362f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_8", 2000507, 4274675, 4274735, 4, { 142.015594f, 22.079769f, 6.271362f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_9", 2000508, 4274676, 4274736, 4, { 158.190201f, 22.079769f, 6.271362f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_10", 2000515, 4274685, 4274737, 4, { 193.987900f, 22.110291f, 42.099609f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_11", 2000516, 4274684, 4274738, 4, { 193.804794f, 22.079769f, 57.938480f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_12", 2000517, 4274683, 4274739, 4, { 193.804794f, 22.079769f, 65.964722f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_13", 2000518, 4274688, 4274740, 4, { 206.317093f, 22.079769f, 42.038570f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_14", 2000519, 4274687, 4274741, 4, { 206.317093f, 22.079769f, 57.968990f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_15", 2000520, 4274686, 4274742, 4, { 206.317093f, 22.079769f, 65.995239f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_16", 2000527, 4274677, 4274743, 4, { 241.992798f, 22.079769f, -6.332520f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_17", 2000528, 4274678, 4274744, 4, { 258.075806f, 22.079769f, -6.332520f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_18", 2000529, 4274679, 4274745, 4, { 265.949493f, 22.079769f, -6.271484f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_19", 2000530, 4274680, 4274746, 4, { 241.931793f, 22.079769f, 6.271362f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_20", 2000531, 4274681, 4274747, 4, { 258.075806f, 22.079769f, 6.271362f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_21", 2000532, 4274682, 4274748, 4, { 266.071503f, 22.079769f, 6.271362f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_22", 2000539, 4274689, 4274749, 4, { 193.804794f, 22.079769f, -34.042912f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_23", 2000540, 4274694, 4274750, 4, { 193.804794f, 22.079769f, -42.008121f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_24", 2000541, 4274693, 4274751, 4, { 193.804794f, 22.079769f, -58.335270f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_25", 2000542, 4274690, 4274752, 4, { 206.317093f, 22.079769f, -33.890320f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_26", 2000543, 4274692, 4274753, 4, { 206.317093f, 22.079769f, -41.916561f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "sgbg_f1d3_a1_gar2_27", 2000544, 4274691, 4274754, 4, { 206.317093f, 22.079769f, -58.274231f }, 0.991760f, 0.000048f, 0); 
    // States -> on (id: 4) off (id: 5) vfx_on (id: 8) 
    instance.addEObj( "Entrance_1", 2000182, 5608623, 5608627, 5, { -156.520401f, -2.800003f, -5.140180f }, 1.000000f, 0.000000f, 0); 
    // States -> vf_lock_on (id: 11) vf_lock_of (id: 12) 
    instance.addEObj( "sgvf_w_lvd_b0118_1", 2002872, 5608617, 5608633, 4, { -154.662003f, -2.800003f, 5.028913f }, 1.000000f, 0.000000f, 0); 
    // States -> vf_bextwall_on (id: 3) vf_bextwall_of (id: 4) 
    instance.addEObj( "unknown_1", 2002735, 0, 5608642, 4, { -151.232193f, -2.822998f, 0.289556f }, 1.174927f, -1.570451f, 0); 
    instance.addEObj( "sgvf_w_lvd_b0118_2", 2005128, 5608619, 5609827, 4, { -112.240501f, -2.800003f, -0.701091f }, 1.000000f, 0.000000f, 0); 
    // States -> vf_bextwall_on (id: 3) vf_bextwall_of (id: 4) 
    instance.addEObj( "unknown_2", 2002735, 0, 5610996, 4, { -46.585720f, 0.045776f, 0.190301f }, 1.179312f, -1.570451f, 0); 
    instance.addEObj( "sgvf_w_lvd_b0118_3", 2002872, 5608620, 5610998, 4, { -46.476231f, 0.000000f, -0.510445f }, 1.000000f, 0.000000f, 0); 
    // States -> vf_bextwall_on (id: 3) vf_bextwall_of (id: 4) 
    instance.addEObj( "Exit_1", 2000139, 0, 5665465, 4, { -7.766907f, -0.015320f, 0.015198f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgvf_f1d3_bo189", 2000561, 4248559, 4248561, 4, { 200.121902f, 21.988159f, -114.000000f }, 1.000000f, 0.000048f, 0); 
    instance.addEObj( "sgbg_col_wall_00", 2000555, 4249099, 4249110, 4, { 200.000000f, 21.988159f, -151.822800f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "Aetherialflow_1", 2000501, 4247838, 4247841, 4, { 200.061005f, 22.000000f, -165.266006f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_col_wall_00_1", 2000556, 4249100, 4249111, 4, { 200.000000f, 21.988159f, -146.822800f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_col_wall_00_2", 2000557, 4249101, 4249112, 4, { 200.000000f, 21.988159f, -141.822800f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_col_wall_00_3", 2000560, 4249104, 4249125, 4, { 200.000000f, 21.988159f, -126.822800f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_col_wall_00_4", 2000602, 4253048, 4253045, 4, { 200.000000f, 21.988159f, -121.822800f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_col_wall_00_5", 2000559, 4249103, 4249124, 4, { 200.000000f, 21.988159f, -131.822800f }, 0.991760f, 0.000048f, 0); 
    instance.addEObj( "sgbg_col_wall_00_6", 2000558, 4249102, 4249114, 4, { 200.000000f, 21.957701f, -136.838501f }, 0.991760f, 0.000048f, 0); 

  }

  void onUpdate( InstanceContent& instance, uint64_t tickCount ) override
  {

  }

  void onEnterTerritory( InstanceContent& instance, Entity::Player& player, uint32_t eventId, uint16_t param1,
                         uint16_t param2 ) override
  {

  }

};

EXPOSE_SCRIPT( BattleintheBigKeep );