#include "synthv1_config.h"
#include "synthv1_dpfui.h"
#include "mxml.h"
#include "synthv1_param.h"

#include <unordered_map>

// Forward decls - MXML callbacks
const char *whitespace_cb(void *cbdata, mxml_node_t *node, mxml_ws_t where);
mxml_type_t set_opaque_type_cb(void *cbdata, mxml_node_t *node);


// Pre-process XML string to remove whitespaces,
// otherwise MXML will report an error.
inline constexpr std::string __trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

synthv1_preset_manager::synthv1_preset_manager(SynthV1PluginUI *uiInstance) : fUi(uiInstance)
{
}

uint32_t synthv1_preset_manager::loadPreset(const char* sPresetXml)
{
    DISTRHO_SAFE_ASSERT_RETURN(fUi != nullptr, 0)

    mxml_node_t *tree = NULL, *nodePreset = NULL, *nodeParams = NULL, *nodeParam = NULL;

    // Initialize a hashmap of parameter list. This helps find out supported param(s) faster.
    // NOTE: Must use std::string for key.
    //       std::unordered_map.find() cannot find out key properly if key type is const char*.
    std::unordered_map<std::string, synthv1::ParamIndex> s_hash;
    if (s_hash.empty()) {
        for (uint32_t i = 0; i < synthv1::NUM_PARAMS; ++i) {
            const synthv1::ParamIndex index = synthv1::ParamIndex(i);
            s_hash.insert({std::string(synthv1_param::paramName(index)), index});
        }
    }

    // Remove whitespace before/after XML string
    std::string sPresetXml_Preprocessed = __trim(std::string(sPresetXml));

    // Load XML string
    tree = mxmlLoadString(tree, NULL, sPresetXml_Preprocessed.c_str());
    DISTRHO_SAFE_ASSERT_RETURN(tree != NULL, 0)

    // Find root node - <preset>
    // TODO: Load tuning.
    nodePreset = mxmlFindElement(tree, tree, "preset", NULL, NULL,
                       MXML_DESCEND_ALL);
    if (!nodePreset)
    {
        d_stderr2("loadPreset: ERROR: Cannot find node 'preset'");
        mxmlDelete(tree);
        return 0;
    }

    // Read preset name
    String preset_name(mxmlElementGetAttr(nodePreset, "name")); // Use DISTRHO::String for robust
    d_stderr("Load preset name: %s", preset_name.buffer());

    // Tell the DSP side to stop synthesizer first.
    // FIXME: Will the UI side wait for the DSP side?
    fUi->_actionBeforeLoadingPreset();

    // Find all parameters
    nodeParams = mxmlFindElement(nodePreset, nodePreset, "params", NULL, NULL, MXML_DESCEND_ALL);
    if (!nodeParams)
    {
        d_stderr2("loadPreset: ERROR: Cannot find node 'params'");
        mxmlDelete(tree);
        return 0;
    }

    nodeParam = mxmlFindElement(nodeParams, nodeParams, "param", NULL, NULL, MXML_DESCEND_ALL);
    while (nodeParam != NULL) {
        // Read parameter index and name
        synthv1::ParamIndex index = synthv1::ParamIndex(atoi(mxmlElementGetAttr(nodeParam, "index")));
        std::string sName(mxmlElementGetAttr(nodeParam, "name"));

        // Check if the parameter supported
        auto _iter = s_hash.find(sName);
        if (_iter == s_hash.end())  // Name is not in supported params
        {
            // Go to next param node
            nodeParam = mxmlFindElement(nodeParam, nodeParams, "param", NULL, NULL, MXML_DESCEND_ALL);
            continue;
        }
        index = _iter->second;  // Map value

        // Read parameter value
        bool whitespace;
        String sValue(mxmlGetText(nodeParam, &whitespace));

        // d_stderr("Param Index: %d, Name: %s, Value: %s", index, sName.c_str(), sValue.buffer());

        const float fValue = atof(sValue);
        fUi->setParameterValue(index, synthv1_param::paramSafeValue(index, fValue));
        fUi->fParamStorage[index] = synthv1_param::paramSafeValue(index, fValue);

        // Go to next param node
        nodeParam = mxmlFindElement(nodeParam, nodeParams, "param", NULL, NULL, MXML_DESCEND_ALL);
    }

    // Cleanup
    mxmlDelete(tree);

    // Tell the DSP side to resume synthesizer
    // FIXME: Will the UI side wait for the DSP side?
    fUi->_actionAfterLoadingPreset();

    // Refresh UI
    fUi->repaint();

    return 1;   // Means OK
}

String synthv1_preset_manager::savePreset(const char* presetName)
{
    DISTRHO_SAFE_ASSERT_RETURN(fUi != nullptr, String(""))

    mxml_node_t *xml;           /* <!DOCTYPE synthv1> */
    mxml_node_t *nodePreset;    /* <preset> */
    mxml_node_t *nodeParams;    /* <params> */
    mxml_node_t *eParam;        /* <param> */

    // MXML option for formatting XML file
    mxml_options_t *options = mxmlOptionsNew();
    mxmlOptionsSetWrapMargin(options, 0);   // Disable word wrap
    mxmlOptionsSetWhitespaceCallback(options, whitespace_cb, /*cbdata*/NULL);   // Add custom formatter

    // FIXME: Does this really necessary on DPF?
    fUi->_requestStabilize();

    xml = mxmlNewDeclarationf(NULL, "DOCTYPE %s", SYNTHV1_TITLE);

    nodePreset = mxmlNewElement(xml, "preset");
    mxmlElementSetAttr(nodePreset, "name", presetName);
    mxmlElementSetAttrf(nodePreset, "version", "%s", CONFIG_BUILD_VERSION);

    nodeParams = mxmlNewElement(nodePreset, "params");
    for (uint32_t i = 0; i < synthv1::NUM_PARAMS; ++i)
    {
        eParam = mxmlNewElement(nodeParams, "param");
		const synthv1::ParamIndex index = synthv1::ParamIndex(i);
        mxmlElementSetAttrf(eParam, "index", "%d", i);
        mxmlElementSetAttr(eParam, "name", synthv1_param::paramName(index));
        mxmlNewTextf(eParam, false, "%f", fUi->fParamStorage[index]);
    }

    String buffer(mxmlSaveAllocString(xml, options));

    mxmlDelete(xml);
    mxmlOptionsDelete(options);

    return buffer;
}

uint32_t synthv1_preset_manager::loadBank(const char* bankXml, synthv1_preset_bank& newBank)
{
    /**
        Bank scheme like:

        <!DOCTYPE synthv1>
        <bank name="{BANK_NAME}" author="{BANK_AUTHOR}">
            <description>__Add some descriptions here.__</description>
            <copyright>__Add copyright info here, if you'd like.</copyright>
            <preset name="{PRESET_1_NAME}" version="{SYNTHV1_VERSION}">
                <!-- Same as a single preset -->
            </preset>
            <preset name="{PRESET_02_NAME}" version="{SYNTHV1_VERSION}">
                <!-- Same as a single preset -->
            </preset>
            ...
        </bank>
     */
    mxml_node_t *tree = NULL;    /* <!DOCTYPE synthv1> */
    mxml_node_t *nodeBank = NULL, *nodeDescription = NULL, *nodeCopyright = NULL;  /* <bank>, <description>, <copyright> */
    mxml_node_t *nodePreset = NULL;   /* <preset> */

    // Remove whitespace before/after XML string
    std::string sBankXml_Preprocessed = __trim(std::string(bankXml));

    // Set opaque node on tag <description> and <copyright>.
    // Opaque node allows you to get all of the text within the tag, with mxmlGetOpaque().
    // NOTE: Simply using mxmlGetText() will only get one word at a time, since this function stops at whitespace.
    mxml_options_t *loader_options = mxmlOptionsNew();
    mxmlOptionsSetTypeCallback(loader_options, set_opaque_type_cb, NULL);

    // Load XML string
    tree = mxmlLoadString(tree, loader_options, sBankXml_Preprocessed.c_str());
    mxmlOptionsDelete(loader_options);

    DISTRHO_SAFE_ASSERT_RETURN(tree != NULL, 0)

    // Find root node - <bank>
    nodeBank = mxmlFindElement(tree, tree, "bank", NULL, NULL,
                       MXML_DESCEND_ALL);
    if (!nodeBank)
    {
        d_stderr2("loadBank: ERROR: Cannot find node 'bank'");
        mxmlDelete(tree);
        return 0;
    }

    // Read bank attrs
    newBank.name = String(mxmlElementGetAttr(nodeBank, "name")); // Use DISTRHO::String for robust
    newBank.author = String(mxmlElementGetAttr(nodeBank, "author"));
    // d_stderr("Load bank: %s, author: %s", newBank.name.buffer(), newBank.author.buffer());

    // Read optional description & copyright info
    nodeDescription = mxmlFindElement(nodeBank, nodeBank, "description", NULL, NULL, MXML_DESCEND_ALL);
    if (nodeDescription)
    {
        newBank.description = String(mxmlGetOpaque(nodeDescription));
    }

    nodeCopyright = mxmlFindElement(nodeBank, nodeBank, "copyright", NULL, NULL, MXML_DESCEND_ALL);
    if (nodeCopyright)
    {
        newBank.copyright = String(mxmlGetOpaque(nodeCopyright));
    }

    // Read preset nodes
    nodePreset = mxmlFindElement(nodeBank, nodeBank, "preset", NULL, NULL, MXML_DESCEND_ALL);
    while (nodePreset != NULL) {
        // Create new preset object
        synthv1_preset newPreset;

        // Read preset name
        newPreset.name = String(mxmlElementGetAttr(nodePreset, "name"));

        // Read and save preset content
        newPreset.presetXml = String("<!DOCTYPE " SYNTHV1_TITLE ">"); // Manually add declalation
        newPreset.presetXml += String(mxmlSaveAllocString(nodePreset, NULL));

        // Push the new preset to our bank
        newBank.presets.push_back(std::move(newPreset));    // Use std::move() for better performance (SURE?)

        // Go to next param node
        nodePreset = mxmlFindElement(nodePreset, nodeBank, "preset", NULL, NULL, MXML_DESCEND_ALL);
    }

    // Cleanup
    mxmlDelete(tree);

    return 1;
}

String synthv1_preset_manager::saveBank(const synthv1_preset_bank& bank)
{
    mxml_node_t *xml;           /* <!DOCTYPE synthv1> */
    mxml_node_t *nodeBank;      /* <bank> */
    mxml_node_t *nodeDescription, *nodeCopyright;   /* <description>, <copyright> */
    mxml_node_t *nodePreset;    /* <preset> */

    // MXML option for formatting XML file
    mxml_options_t *options = mxmlOptionsNew();
    mxmlOptionsSetWrapMargin(options, 0);   // Disable word wrap
    mxmlOptionsSetWhitespaceCallback(options, whitespace_cb, /*cbdata*/NULL);   // Add custom formatter

    xml = mxmlNewDeclarationf(NULL, "DOCTYPE %s", SYNTHV1_TITLE);

    nodeBank = mxmlNewElement(xml, "bank");
    mxmlElementSetAttr(nodeBank, "name", bank.name);
    mxmlElementSetAttr(nodeBank, "version", bank.author);

    if (bank.description.length() > 0)
    {
        nodeDescription = mxmlNewElement(nodeBank, "description");
        mxmlNewOpaque(nodeDescription, bank.description);
    }

    if (bank.copyright.length() > 0)
    {
        nodeCopyright = mxmlNewElement(nodeBank, "copyright");
        mxmlNewOpaque(nodeCopyright, bank.copyright);
    }

    for (auto preset : bank.presets)
    {
        // Re-serialize preset XML
        mxml_node_t *presetTree = mxmlLoadString(NULL, NULL, preset.presetXml);

        // Find out the preset node
        nodePreset = mxmlFindElement(presetTree, presetTree, "preset", NULL, NULL,
                       MXML_DESCEND_ALL);
        
        // Sync the attributes
        mxmlElementSetAttr(nodePreset, "name", preset.name);
        mxmlElementSetAttrf(nodePreset, "version", "%s", SYNTHV1_TITLE);

        // Append preset node to the bank XML
        mxmlAdd(nodeBank, MXML_ADD_AFTER, NULL, nodePreset);

        // Cleanup
        mxmlDelete(presetTree);
    }

    String buffer(mxmlSaveAllocString(xml, options));

    mxmlDelete(xml);
    mxmlOptionsDelete(options);

    return buffer;
}

// XML formatter callback.
const char *whitespace_cb(void *cbdata, mxml_node_t *node, mxml_ws_t where)
{
    String decl(mxmlGetDeclaration(node));
    if (decl.contains("DOCTYPE"))
    {
        if (where == MXML_WS_AFTER_OPEN)
            return ("\n");
    }

    String elem(mxmlGetElement(node));
    if (elem.contains("preset"))
    {
        if (where == MXML_WS_AFTER_OPEN)
            return ("\n");
    }
    else if (elem.contains("params"))
    {
        if (where == MXML_WS_BEFORE_OPEN || where == MXML_WS_BEFORE_CLOSE)
            return (" ");
        else if (where == MXML_WS_AFTER_OPEN)
            return ("\n");
    }
    else if (elem.contains("param"))
    {
        if (where == MXML_WS_BEFORE_OPEN)
        return ("  ");
    }

    if (where == MXML_WS_AFTER_CLOSE)
        return ("\n");

    return (NULL);
}

mxml_type_t set_opaque_type_cb(void *cbdata, mxml_node_t *node) {
    String name(mxmlGetElement(node));

    // Check node name. Only <description> and <copyright> should be opaque string.
    if (strcmp(name, "description") == 0 || strcmp(name, "copyright") == 0) {
        return MXML_TYPE_OPAQUE;
    }

    // 对于其他节点，返回MXML_TEXT
    return MXML_TYPE_TEXT;
}
