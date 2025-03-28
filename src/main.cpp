#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>

using namespace geode::prelude;

auto mod = Mod::get();

const std::unordered_map<int, ccColor3B> colorMap = {
    {901, {255, 0, 255}}, {3006, {255, 0, 255}}, {3011, {255, 0, 255}},  
    {1616, {163, 0, 86}}, {1006, {255, 255, 0}}, {3010, {255, 255, 0}},  
    {3015, {255, 255, 0}}, {3617, {255, 255, 0}}, {3620, {255, 255, 0}},  
    {2015, {255, 255, 0}}, {1007, {0, 255, 255}}, {3009, {0, 255, 255}},  
    {3014, {0, 255, 255}}, {3615, {0, 255, 255}}, {3619, {0, 255, 255}},  
    {2066, {0, 255, 255}}, {1049, {255, 63, 63}}, {1268, {35, 204, 127}},  
    {1346, {127, 127, 255}}, {3007, {127, 127, 255}}, {3012, {127, 127, 255}},  
    {1913, {127, 127, 255}}, {2067, {63, 191, 255}}, {3008, {63, 191, 255}},  
    {3013, {63, 191, 255}}, {2062, {63, 191, 255}}, {1347, {255, 127, 127}},  
    {1914, {255, 127, 127}}, {1585, {255, 183, 0}}, {1814, {255, 255, 127}},  
    {3016, {204, 255, 199}}, {3660, {204, 255, 199}}, {3661, {204, 255, 199}},  
    {1595, {0, 137, 178}}, {1611, {255, 183, 252}}, {1811, {255, 127, 255}},  
    {2999, {255, 127, 255}}, {3612, {255, 127, 255}}, {1817, {255, 109, 0}},  
    {3618, {255, 100, 0}}, {1912, {63, 127, 255}}, {2068, {165, 94, 255}},  
    {3607, {165, 94, 255}}, {3608, {150, 150, 150}}, {3602, {255, 255, 255}},  
    {3603, {255, 255, 255}}, {3604, {255, 255, 255}}, {1615, {255, 255, 255}},  
    {3613, {255, 255, 255}}, {3662, {255, 255, 255}}, {1935, {153, 127, 153}},  
    {1932, {101, 255, 255}}, {3606, {127, 255, 255}}, {1815, {83, 66, 250}},  
    {3609, {83, 191, 255}}, {1812, {204, 101, 101}}, {3600, {0, 0, 0}},  
    {3022, {104, 226, 255}}, {2902, {104, 226, 255}}, {899, {255, 255, 255}}
};

std::string floatToFormattedString(float num, int round) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(round) << num;
    std::string string = ss.str();
    string.erase(string.find_last_not_of('0') + 1, std::string::npos);
    if (string.back() == '.') string.pop_back();
    return string;
}

class $modify(Editor, LevelEditorLayer) {
    struct Fields {
        CCLayer* batchLayer;
        CCLayer* contextMenuLayer;
        CCNode* contextMenu;
        int lastObjectCount;
        float menuScaleMultiplier = 1.5f;
        EffectGameObject* obj;
    };

	bool init(GJGameLevel* p0, bool p1) {	
		if (!LevelEditorLayer::init(p0, p1)) return false;
        if (auto shaderLayer = this->getChildByType<ShaderLayer>(0)) m_fields->batchLayer = shaderLayer->getChildByType<CCNode>(1)->getChildByType<CCLayer>(0);
        else m_fields->batchLayer = this->getChildByType<CCNode>(1)->getChildByType<CCLayer>(0);

        auto contextMenuLayer = CCLayer::create();
        contextMenuLayer->setPosition(ccp(0.0f, 0.0f));
        contextMenuLayer->setZOrder(9999);
        contextMenuLayer->setID("trigger-context-menu-layer");
        m_fields->batchLayer->addChild(contextMenuLayer);
        m_fields->contextMenuLayer = contextMenuLayer;

        auto menuNode = CCNode::create();
        menuNode->setVisible(false);
        auto menuButtonMenu = CCMenu::create();
        menuButtonMenu->setContentSize(CCSize(75.0f, 75.0f));
        menuButtonMenu->setPosition(ccp(0.0f, 0.0f));
        auto menuFill = CCSprite::create("menusolid.png"_spr);
        menuFill->setPosition(ccp(0.0f, 0.0f));
        menuFill->setScale(0.5f);
        auto menuBorder = CCSprite::create("menuborder.png"_spr);
        menuBorder->setPosition(ccp(0.0f, 0.0f));
        menuBorder->setScale(0.5f);
        menuNode->addChild(menuFill);
        menuNode->addChild(menuBorder);
        menuNode->addChild(menuButtonMenu);
        m_fields->contextMenuLayer->addChild(menuNode);        
        m_fields->contextMenu = menuNode;
        return true;
    }

    void createContextMenu(GameObject* obj) {
        if (!m_fields->contextMenu || !obj) return;
        auto menuNode = m_fields->contextMenu;
        auto id = obj->m_objectID;
        if (id == 901) setupContextMenu(obj, menuNode, getColorFromID(id), "Move", "Move X", "Move Y", "Duration", "Group", "Spawn", "Multi");
        if (id == 1007) setupContextMenu(obj, menuNode, getColorFromID(id), "Alpha", "Opacity", "Duration", "Group", "Null", "Spawn", "Multi");
        if (id == 1346) setupContextMenu(obj, menuNode, getColorFromID(id), "Rotate", "Rotation", "Duration", "Group", "Center Group", "Spawn", "Multi");
        if (id == 1049) setupContextMenu(obj, menuNode, getColorFromID(id), "Toggle", "Group", "Activate Group", "Null", "Null", "Spawn", "Multi");
        if (id == 2067) setupContextMenu(obj, menuNode, getColorFromID(id), "Scale", "Scale X", "Scale Y", "Duration", "Group", "Spawn", "Multi");
        if (id == 1268) setupContextMenu(obj, menuNode, getColorFromID(id), "Spawn", "Group", "Spawn Delay", "Null", "Null", "Spawn", "Multi");
        if (id == 1817) setupContextMenu(obj, menuNode, getColorFromID(id), "Pickup", "Item ID", "Count", "Override", "Null", "Spawn", "Multi");
        if (id == 1611) setupContextMenu(obj, menuNode, getColorFromID(id), "Count", "Item ID", "Group", "Count", "Activate Group", "Spawn", "Multi");
        if (id == 1811) setupContextMenu(obj, menuNode, getColorFromID(id), "Instant Count", "Item ID", "Group", "Count", "Activate Group", "Spawn", "Multi");
        if (id == 1816) setupContextMenu(obj, menuNode, getColorFromID(id), "Collision Block", "Collision ID", "Dynamic Block", "Null", "Null", "Spawn", "Multi");
        if (id == 1815) setupContextMenu(obj, menuNode, getColorFromID(id), "Collision", "Collision ID", "Collision ID 2", "Group", "Activate Group", "Spawn", "Multi");
        if (id == 3609) setupContextMenu(obj, menuNode, getColorFromID(id), "Instant Collision", "Collision ID", "Collision ID 2", "True", "False", "Spawn", "Multi");
        if (id == 3016) setupContextMenu(obj, menuNode, getColorFromID(id), "Advanced Follow", "Follow X/Y Only", "Group", "Follow Group", "Null", "Spawn", "Multi");
        if (id == 1347) setupContextMenu(obj, menuNode, getColorFromID(id), "Follow", "Duration", "Group", "Center Group", "Null", "Spawn", "Multi");
        if (id == 2909) setupContextMenu(obj, menuNode, getColorFromID(id), "Glitch", "Strength", "Speed", "Duration", "Null", "Spawn", "Multi");
        if (id == 2910) setupContextMenu(obj, menuNode, getColorFromID(id), "Chromatic", "Shader Target X", "Shader Target Y", "Shader Use X/Y", "Duration", "Spawn", "Multi");
        if (id == 2911) setupContextMenu(obj, menuNode, getColorFromID(id), "Chroma Glitch", "Strength", "Speed", "RGB Off", "Duration", "Spawn", "Multi");
        if (id == 2912) setupContextMenu(obj, menuNode, getColorFromID(id), "Pixelate", "Shader Target X", "Shader Target Y", "Shader Use X/Y", "Duration", "Spawn", "Multi");
        if (id == 2913) setupContextMenu(obj, menuNode, getColorFromID(id), "Lens Circle", "Size", "Strength", "Fade", "Duration", "Spawn", "Multi");
        if (id == 2914) setupContextMenu(obj, menuNode, getColorFromID(id), "Radial Blur", "Size", "Intensity", "Fade", "Duration", "Spawn", "Multi");
        if (id == 2915) setupContextMenu(obj, menuNode, getColorFromID(id), "Motion Blur", "Shader Target X", "Shader Target Y", "Shader Use X/Y", "Duration", "Spawn", "Multi");
        if (id == 2916) setupContextMenu(obj, menuNode, getColorFromID(id), "Buldge", "Buldge", "Radius", "Duration", "Null", "Spawn", "Multi");
        if (id == 2917) setupContextMenu(obj, menuNode, getColorFromID(id), "Pinch", "Radius", "Modifier", "Shader Use X/Y", "Duration", "Spawn", "Multi");
        if (id == 2924) setupContextMenu(obj, menuNode, getColorFromID(id), "Split Screen", "Shader Target X", "Shader Target Y", "Shader Use X/Y", "Duration", "Spawn", "Multi");
        // if (id == ) setupContextMenu(obj, menuNode, getColorFromID(id), "", "", "", "", "", "Spawn", "Multi");
        
    }

    void setupContextMenu(GameObject* selectedObj, CCNode* menuNode, ccColor3B col, const char* menuLabelText, std::string field1, std::string field2, std::string field3, std::string field4, std::string toggle1, std::string toggle2) {
        auto obj = static_cast<EffectGameObject*>(selectedObj);
        auto menuFill = menuNode->getChildByType<CCSprite>(0);
        auto menu = menuNode->getChildByType<CCMenu>(0);
        menuNode->setVisible(true);
        menuNode->setScale(m_fields->menuScaleMultiplier);
        menuFill->setColor(col);
        menu->removeAllChildren();
        m_fields->obj = obj;

        auto menuLabel = CCLabelBMFont::create(menuLabelText, "bigFont.fnt");
        menuLabel->setPosition(ccp(0.0f, 29.0f));
        menuLabel->setScale(0.225f);
        menu->addChild(menuLabel);

        auto editObjectShortcut = CCMenuItemSpriteExtra::create(CCSprite::create("editobjectshortcut.png"_spr), this, menu_selector(Editor::onEditObjectShortcut));
        editObjectShortcut->setPosition(ccp(0.0f, -23.5f));
        editObjectShortcut->setScale(0.4f);
        editObjectShortcut->m_baseScale = 0.4f;
        menu->addChild(editObjectShortcut);
        
        std::vector<CCPoint> pos {
            ccp(-17.5f, 13.0f), ccp(17.5f, 13.0f), ccp(-17.5f, -5.0f), ccp(17.5f, -5.0f), ccp(-22.0f, -23.5f), ccp(22.0f, -23.5f)
        };

        // could this maybe be refactored, yes, does this make my life 5000 times easier, also yes
        int loopIndex = -1;
        for (std::string fieldID : {field1, field2, field3, field4}) {
            loopIndex++;
            if (fieldID == "Null") continue;
            auto field = TextInput::create(60, "");
            field->setScale(0.3f);
            field->setFilter("1234567890");
            field->setPosition(pos[loopIndex]);
            
            auto label = CCLabelBMFont::create(fieldID.c_str(), "chatFont.fnt");
            label->setScale(0.3f);
            label->setColor(menuFill->getColor() == ccc3(255, 255, 255) ? ccc3(0, 0, 0) : ccc3(255, 255, 255));
            label->setPosition(ccp(pos[loopIndex].x, pos[loopIndex].y + 7.5f));

            if (fieldID == "Move X") {
                field->setFilter("-1234567890");
                field->setString(floatToFormattedString(std::floor(std::round(obj->m_moveOffset.x) / (obj->m_smallStep ? 1 : 3)), 1).c_str(), false);
                field->setCallback([obj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) {
                        obj->m_moveOffset.x = std::stoi(input) * (obj->m_smallStep ? 1 : 3);
                    }
                });
            }
            if (fieldID == "Move Y") {
                field->setFilter("-1234567890");
                field->setString(floatToFormattedString(std::floor(obj->m_moveOffset.y / (obj->m_smallStep ? 1 : 3)), 1).c_str(), false);
                field->setCallback([obj] (const std::string& input) {
                    if (!input.empty()) obj->m_moveOffset.y = std::stoi(input) * (obj->m_smallStep ? 1 : 3);
                });
            }
            if (fieldID == "Duration") {
                field->setFilter("1234567890.");
                field->setString(floatToFormattedString(obj->m_duration, 2).c_str(), false);
                field->setCallback([obj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) obj->m_duration = std::stof(input);
                });
            }
            if (fieldID == "Spawn Delay") {
                auto spawnObj = static_cast<SpawnTriggerGameObject*>(obj);
                field->setFilter("1234567890.");
                field->setString(floatToFormattedString(spawnObj->m_spawnDelay, 4).c_str(), false);
                field->setCallback([spawnObj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) spawnObj->m_spawnDelay = std::stof(input);
                });
            }
            if (fieldID == "Group" || fieldID == "True") {
                field->setMaxCharCount(4);
                field->setString(std::to_string(obj->m_targetGroupID).c_str(), false);
                field->setCallback([field, obj] (const std::string& input) {
                    if (!input.empty()) {
                        obj->m_targetGroupID = std::stoi(input);
                        obj->getObjectLabel()->setString(std::to_string(std::stoi(input)).c_str()); // really dumb way to get rid of 0s at the start
                    }
                });
            }
            if (fieldID == "Center Group" || fieldID == "False") {
                field->setMaxCharCount(4);
                field->setString(std::to_string(obj->m_centerGroupID).c_str(), false);
                field->setCallback([field, obj] (const std::string& input) {
                    if (!input.empty()) obj->m_centerGroupID = std::stoi(input);
                });
            }
            if (fieldID == "Item ID") {
                field->setMaxCharCount(4);
                field->setString(std::to_string(obj->m_itemID).c_str(), false);
                field->setCallback([field, obj] (const std::string& input) {
                    if (!input.empty()) {
                        obj->m_itemID = std::stoi(input);
                        if (obj->m_objectID == 1817) obj->getObjectLabel()->setString(std::to_string(std::stoi(input)).c_str());
                    }
                });
            }
            if (fieldID == "Item ID 2" || fieldID == "Collision ID 2") {
                field->setMaxCharCount(4);
                field->setString(std::to_string(obj->m_itemID2).c_str(), false);
                field->setCallback([field, obj] (const std::string& input) {
                    if (!input.empty()) obj->m_itemID2 = std::stoi(input);
                });
            }
            if (fieldID == "Collision ID") {
                field->setMaxCharCount(4);
                field->setFilter("1234567890-p");
                field->setString(std::to_string(obj->m_itemID).c_str(), false);
                if (obj->m_targetPlayer1 && obj->m_targetPlayer2) field->setString("P (-3)", false);
                else if (obj->m_targetPlayer1) field->setString("P1 (-1)", false);
                else if (obj->m_targetPlayer2) field->setString("P2 (-2)", false);
                if (obj->m_followCPP) field->setString("PP (-4)", false);
                field->setCallback([field, obj] (const std::string& input) {
                    if (input.length() >= 5) field->setString("", false);
                    if (input == "0p" || input == "p0") field->setString("p", false);
                    if (input == "-4" || input == "pp") {
                        obj->m_targetPlayer1 = false;
                        obj->m_targetPlayer2 = false;
                        obj->m_followCPP = true;
                        field->setString("PP (-4)", false);
                    }
                    if (input == "-3") {
                        obj->m_targetPlayer1 = true;
                        obj->m_targetPlayer2 = true;
                        obj->m_followCPP = false;
                        field->setString("P (-3)", false);
                    }
                    if (input == "-1" || input == "p1") {
                        obj->m_targetPlayer1 = true;
                        obj->m_targetPlayer2 = false;
                        obj->m_followCPP = false;
                        field->setString("P1 (-1)", false);
                    }
                    if (input == "-2" || input == "p2") {
                        obj->m_targetPlayer1 = false;
                        obj->m_targetPlayer2 = true;
                        obj->m_followCPP = false;
                        field->setString("P2 (-2)", false);
                    }
                    if (!input.empty() && input.find_first_of("p-() ") == std::string::npos) {
                        obj->m_targetPlayer1 = false;
                        obj->m_targetPlayer2 = false;
                        obj->m_followCPP = false;
                        obj->m_itemID = std::stoi(input);
                    }
                });
            }
            if (fieldID == "Count") {
                auto countObj = static_cast<CountTriggerGameObject*>(obj);
                field->setString(std::to_string(countObj->m_pickupCount).c_str(), false);
                field->setCallback([field, countObj] (const std::string& input) {
                    if (!input.empty()) countObj->m_pickupCount = std::stoi(input);
                });
            }
            if (fieldID == "Override") {
                auto countObj = static_cast<CountTriggerGameObject*>(obj);
                field->setFilter("01truefals");
                field->setString(countObj->m_isOverride ? "True (1)" : "False (0)", false);
                field->setCallback([field, countObj] (const std::string& input) {
                    if (input.length() >= 6) field->setString("", false);
                    if (input == "0" || input == "false") {
                        countObj->m_isOverride = false;
                        field->setString("False (0)", false);
                    }
                    if (input == "1" || input == "true") { 
                        countObj->m_isOverride = true;
                        field->setString("True (1)", false);
                    }
                });
            }
            if (fieldID == "Dynamic Block") {
                auto countObj = static_cast<CountTriggerGameObject*>(obj);
                field->setFilter("01truefals");
                field->setString(countObj->m_isDynamicBlock ? "True (1)" : "False (0)", false);
                field->setCallback([field, countObj] (const std::string& input) {
                    if (input.length() >= 6) field->setString("", false);
                    if (input == "0" || input == "false") {
                        countObj->m_isDynamicBlock = false;
                        field->setString("False (0)", false);
                    }
                    if (input == "1" || input == "true") { 
                        countObj->m_isDynamicBlock = true;
                        field->setString("True (1)", false);
                    }
                });
            }
            if (fieldID == "Follow X/Y Only") {
                auto followObj = static_cast<AdvancedFollowTriggerObject*>(obj);
                field->setFilter("012xyan");
                if (followObj->m_xOnly) field->setString("X (1)", false);
                else if (followObj->m_yOnly) field->setString("Y (2)", false);
                else field->setString("Any (0)", false);
                field->setCallback([field, followObj] (const std::string& input) {
                    if (input.length() >= 4) field->setString("", false);
                    if (input == "0" || input == "any") {
                        followObj->m_xOnly = false;
                        followObj->m_yOnly = false;
                        field->setString("Any (0)", false);
                    }
                    if (input == "1" || input == "x") {
                        followObj->m_xOnly = true;
                        followObj->m_yOnly = false;
                        field->setString("X (1)", false);
                    }
                    if (input == "2" || input == "y") {
                        followObj->m_xOnly = false;
                        followObj->m_yOnly = true;
                        field->setString("Y (2)", false);
                    }
                });
            }
            if (fieldID == "Follow Group") {
                field->setMaxCharCount(4);
                field->setFilter("1234567890-pc");
                field->setString(std::to_string(obj->m_centerGroupID).c_str(), false);
                if (obj->m_targetPlayer1) field->setString("P1 (-1)", false);
                if (obj->m_targetPlayer2) field->setString("P2 (-2)", false);
                if (obj->m_followCPP) field->setString("C (-3)", false);
                field->setCallback([field, obj] (const std::string& input) {
                    if (input.length() >= 5) field->setString("", false);
                    if (input == "0p" || input == "p0") field->setString("p", false);
                    if (input == "0c" || input == "c0" || input == "-3" || input == "c") {
                        obj->m_targetPlayer1 = false;
                        obj->m_targetPlayer2 = false;
                        obj->m_followCPP = true;
                        field->setString("C (-3)", false);
                    }
                    if (input == "-1" || input == "p1") {
                        obj->m_targetPlayer1 = true;
                        obj->m_targetPlayer2 = false;
                        obj->m_followCPP = false;
                        field->setString("P1 (-1)", false);
                    }
                    if (input == "-2" || input == "p2") {
                        obj->m_targetPlayer1 = false;
                        obj->m_targetPlayer2 = true;
                        obj->m_followCPP = false;
                        field->setString("P2 (-2)", false);
                    }
                    if (!input.empty() && input.find_first_of("pc-() ") == std::string::npos) {
                        obj->m_targetPlayer1 = false;
                        obj->m_targetPlayer2 = false;
                        obj->m_followCPP = false;
                        obj->m_centerGroupID = std::stoi(input);
                    }
                });
            }
            if (fieldID == "Rotation") {
                field->setFilter("1234567890.");
                field->setString(floatToFormattedString(obj->m_rotationDegrees, 2).c_str(), false);
                field->setCallback([obj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) obj->m_rotationDegrees = std::stof(input);
                });
            }
            if (fieldID == "Opacity") {
                field->setFilter("1234567890.");
                field->setString(floatToFormattedString(obj->m_opacity, 2).c_str(), false);
                field->setCallback([obj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) {
                        if (std::stof(input) <= 1) obj->m_opacity = std::stof(input);
                    }
                });
            }  
            if (fieldID == "Activate Group") {
                field->setFilter("01truefals");
                field->setString(obj->m_activateGroup ? "True (1)" : "False (0)", false);
                field->setCallback([field, obj] (const std::string& input) {
                    if (input.length() >= 6) field->setString("", false);
                    if (input == "0" || input == "false") {
                        obj->m_activateGroup = false;
                        if (auto spr = obj->getChildByType<CCSprite>(0)) spr->setColor(ccc3(255, 63, 63));
                        field->setString("False (0)", false);
                    }
                    if (input == "1" || input == "true") { 
                        obj->m_activateGroup = true;
                        if (auto spr = obj->getChildByType<CCSprite>(0)) spr->setColor(ccc3(0, 255, 127));
                        field->setString("True (1)", false);
                    }
                });
            }
            if (fieldID == "Scale X") {
                auto scaleObj = static_cast<TransformTriggerGameObject*>(obj);
                field->setFilter("1234567890.-");
                field->setString(floatToFormattedString(scaleObj->m_objectScaleX, 3).c_str(), false);
                field->setCallback([scaleObj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) scaleObj->m_objectScaleX = std::stof(input);
                });
            }
            if (fieldID == "Scale Y") {
                auto scaleObj = static_cast<TransformTriggerGameObject*>(obj);
                field->setFilter("1234567890.-");
                field->setString(floatToFormattedString(scaleObj->m_objectScaleX, 3).c_str(), false);
                field->setCallback([scaleObj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) scaleObj->m_objectScaleX = std::stof(input);
                });
            }
            if (fieldID == "Intensity" || fieldID == "Strength" || fieldID == "Buldge") {
                auto shaderObj = static_cast<ShaderGameObject*>(obj);
                field->setFilter("1234567890.-");
                field->setString(floatToFormattedString(shaderObj->m_strength, 2).c_str(), false);
                field->setCallback([shaderObj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) shaderObj->m_strength = std::stof(input);
                });
            }
            if (fieldID == "Size" || fieldID == "Modifier") {
                auto shaderObj = static_cast<ShaderGameObject*>(obj);
                field->setFilter("1234567890.-");
                field->setString(floatToFormattedString(shaderObj->m_waveWidth, shaderObj->m_objectID == 2917 ? 3 : 2).c_str(), false);
                field->setCallback([shaderObj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) shaderObj->m_waveWidth = std::stof(input);
                });
            }
            if (fieldID == "Radius") {
                auto shaderObj = static_cast<ShaderGameObject*>(obj);
                field->setFilter("1234567890.-");
                field->setString(floatToFormattedString(shaderObj->m_objectID == 2917 ? shaderObj->m_maxSize : shaderObj->m_targetX, shaderObj->m_objectID == 2917 ? 3 : 2).c_str(), false);
                field->setCallback([shaderObj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) shaderObj->m_objectID == 2917 ? shaderObj->m_maxSize : shaderObj->m_targetX  = std::stof(input);
                });
            }
            if (fieldID == "Speed") {
                auto shaderObj = static_cast<ShaderGameObject*>(obj);
                field->setFilter("1234567890.");
                field->setString(floatToFormattedString(shaderObj->m_speed, 2).c_str(), false);
                field->setCallback([shaderObj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) shaderObj->m_speed = std::stof(input);
                });
            }
            if (fieldID == "Fade") {
                auto shaderObj = static_cast<ShaderGameObject*>(obj);
                field->setFilter("1234567890.-");
                field->setString(floatToFormattedString(shaderObj->m_fadeIn, 2).c_str(), false);
                field->setCallback([shaderObj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) shaderObj->m_fadeIn = std::stof(input);
                });
            }
            if (fieldID == "RGB Off") {
                auto shaderObj = static_cast<ShaderGameObject*>(obj);
                field->setFilter("1234567890.-");
                field->setString(floatToFormattedString(shaderObj->m_timeOff, 2).c_str(), false);
                field->setCallback([shaderObj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) shaderObj->m_timeOff = std::stof(input);
                });
            }
            if (fieldID == "Shader Target X") {
                auto shaderObj = static_cast<ShaderGameObject*>(obj);
                field->setFilter("1234567890.-");
                field->setString(floatToFormattedString(shaderObj->m_targetX, 2).c_str(), false);
                field->setCallback([shaderObj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) shaderObj->m_targetX = std::stof(input);
                });
            }
            if (fieldID == "Shader Target Y") {
                auto shaderObj = static_cast<ShaderGameObject*>(obj);
                field->setFilter("1234567890.-");
                field->setString(floatToFormattedString(shaderObj->m_targetY, 2).c_str(), false);
                field->setCallback([shaderObj] (const std::string& input) {
                    if (input.find_first_of("1234567890") != std::string::npos) shaderObj->m_targetY = std::stof(input);
                });
            }
            if (fieldID == "Shader Use X/Y") { // pinch shader so fucked
                auto shaderObj = static_cast<ShaderGameObject*>(obj);
                field->setFilter("0123xynoebth");
                field->setString("None (0)", false);
                if ((shaderObj->m_objectID == 2917 ? shaderObj->m_useY : shaderObj->m_useX) && (shaderObj->m_objectID == 2917 ? shaderObj->m_snapGrid : shaderObj->m_useY)) field->setString("Both (3)", false);
                else if (shaderObj->m_objectID == 2917 ? shaderObj->m_useY : shaderObj->m_useX) field->setString("X (1)", false);
                else if (shaderObj->m_objectID == 2917 ? shaderObj->m_snapGrid : shaderObj->m_useY) field->setString("Y (2)", false);
                field->setCallback([field, shaderObj] (const std::string& input) {
                    if (input.length() >= 4) field->setString("", false);
                    if (input == "0" || input == "none") {
                        (shaderObj->m_objectID == 2917 ? shaderObj->m_useY : shaderObj->m_useX) = false;
                        (shaderObj->m_objectID == 2917 ? shaderObj->m_snapGrid : shaderObj->m_useY) = false;
                        field->setString("None (0)", false);
                    }
                    if (input == "1" || input == "x") {
                        (shaderObj->m_objectID == 2917 ? shaderObj->m_useY : shaderObj->m_useX) = true;
                        (shaderObj->m_objectID == 2917 ? shaderObj->m_snapGrid : shaderObj->m_useY) = false;
                        field->setString("X (1)", false);
                    }
                    if (input == "2" || input == "y") {
                        (shaderObj->m_objectID == 2917 ? shaderObj->m_useY : shaderObj->m_useX) = false;
                        (shaderObj->m_objectID == 2917 ? shaderObj->m_snapGrid : shaderObj->m_useY) = true;
                        field->setString("Y (2)", false);
                    }
                    if (input == "3" || input == "both") {
                        (shaderObj->m_objectID == 2917 ? shaderObj->m_useY : shaderObj->m_useX) = true;
                        (shaderObj->m_objectID == 2917 ? shaderObj->m_snapGrid : shaderObj->m_useY) = true;
                        field->setString("Both (3)", false);
                    }
                });
            }

            menu->addChild(field);
            menu->addChild(label);
        }

        for (std::string toggleID : {toggle1, toggle2}) {
            loopIndex++;
            if (toggleID == "Null") continue;
            auto toggler = CCMenuItemToggler::create(CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
            CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"), this, menu_selector(Editor::onContextMenuToggler));
            if (toggleID == "Spawn") toggler->toggle(obj->m_isSpawnTriggered);
            if (toggleID == "Multi") toggler->toggle(obj->m_isMultiTriggered);
            if (toggleID == "Touch") toggler->toggle(obj->m_isTouchTriggered);
            toggler->setScale(0.325f);
            toggler->setPosition(pos[loopIndex]);
            auto valueNode = CCNode::create();
            valueNode->setID(toggleID);
            toggler->addChild(valueNode);
            menu->addChild(toggler);
        }
    }

    void onContextMenuToggler(CCObject* sender) {
        auto toggleID = static_cast<CCNode*>(sender)->getChildByType<CCNode*>(2)->getID();
        if (auto obj = m_fields->obj) {
            if (toggleID == "Spawn") obj->m_isSpawnTriggered = !obj->m_isSpawnTriggered;
            if (toggleID == "Multi") obj->m_isMultiTriggered = !obj->m_isMultiTriggered;
            if (toggleID == "Touch") obj->m_isTouchTriggered = !obj->m_isTouchTriggered;
        }
    }

    void onEditObjectShortcut(CCObject* sender) {
        m_editorUI->m_editObjectBtn->activate();
    }

    void hideContextMenu() {
        if (auto contextMenu = m_fields->contextMenu) {
            contextMenu->setVisible(false);
            contextMenu->setPosition(0, 0);
        }
    }

    void updateEditor(float p0) {
        LevelEditorLayer::updateEditor(p0);
        auto fields = m_fields.self();
        auto objCount = m_objects->count();
        if (fields->lastObjectCount > objCount) hideContextMenu();
        fields->lastObjectCount = objCount;
        if (auto objs = EditorUI::get()->getSelectedObjects()) {
            if (objs->count() == 1) {
                if (auto contextMenu = fields->contextMenu) if (contextMenu->isVisible()) {
                    auto objPos = static_cast<GameObject*>(objs->objectAtIndex(0))->getPosition();
                    contextMenu->setPosition(ccp(objPos.x, objPos.y + (50.0f * fields->menuScaleMultiplier)));
                }
            }
        }
    }

    ccColor3B getColorFromID(int id) {
        if (auto entry = colorMap.find(id); entry != colorMap.end()) {
            return entry->second;
        }
        return {255, 255, 255};
    }
};

class $modify(EditUI, EditorUI) {
    struct Fields {
        GameObject* selectedObjectCache; // ensure it selects the right object
    };

    void selectObject(GameObject* p0, bool p1) {
        EditorUI::selectObject(p0, p1);
        if (typeinfo_cast<EffectGameObject*>(p0)) {
            m_fields->selectedObjectCache = p0;
            this->scheduleOnce(schedule_selector(EditUI::delayedCreateContextMenu), 0);
        }
    }
    void selectObjects(CCArray* p0, bool p1) {
        EditorUI::selectObjects(p0, p1);
        static_cast<Editor*>(LevelEditorLayer::get())->hideContextMenu();
    }

    void delayedCreateContextMenu(float dt) {
        if (auto obj = m_fields->selectedObjectCache) static_cast<Editor*>(LevelEditorLayer::get())->createContextMenu(obj);
    }
    
    void deselectObject(GameObject* p0) {
        EditorUI::deselectObject(p0);
        static_cast<Editor*>(LevelEditorLayer::get())->hideContextMenu();
    }

    void deselectAll() {
        EditorUI::deselectAll();
        static_cast<Editor*>(LevelEditorLayer::get())->hideContextMenu();
    }
};

class $modify(TriggerPopup, SetupTriggerPopup) {
    void onClose(CCObject* sender) {
        SetupTriggerPopup::onClose(sender);
        if (auto editor = static_cast<Editor*>(LevelEditorLayer::get())) if (!m_gameObjects) editor->createContextMenu(m_gameObject);
    }
};