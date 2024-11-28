#include "synthv1_config.h"
#include "synthv1_dpfui.h"
#include "mxml.h"
#include "synthv1_param.h"

#include <unordered_map>

// Forward decls.
const char *
whitespace_cb(void *cbdata, mxml_node_t *node, mxml_ws_t where);

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

uint32_t SynthV1PluginUI::loadPreset(const char* sPresetXml)
{
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
    _actionBeforeLoadingPreset();

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
        this->setParameterValue(index, synthv1_param::paramSafeValue(index, fValue));
        fParamStorage[index] = synthv1_param::paramSafeValue(index, fValue);

        // Go to next param node
        nodeParam = mxmlFindElement(nodeParam, nodeParams, "param", NULL, NULL, MXML_DESCEND_ALL);
    }

    // Cleanup
    mxmlDelete(tree);

    // Tell the DSP side to resume synthesizer
    // FIXME: Will the UI side wait for the DSP side?
    _actionAfterLoadingPreset();

    // Refresh UI
    repaint();

    return 1;   // Means OK
}

String SynthV1PluginUI::savePreset(const char* presetName)
{
    mxml_node_t *xml;           /* <!DOCTYPE synthv1> */
    mxml_node_t *nodePreset;    /* <preset> */
    mxml_node_t *nodeParams;    /* <params> */
    mxml_node_t *eParam;     /* <param> */

    // MXML option for formatting XML file
    mxml_options_t *options = mxmlOptionsNew();
    mxmlOptionsSetWrapMargin(options, 0);   // Disable word wrap
    mxmlOptionsSetWhitespaceCallback(options, whitespace_cb, /*cbdata*/NULL);   // Add custom formatter

    // FIXME: Does this really necessary on DPF?
    _requestStabilize();

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
        mxmlNewTextf(eParam, false, "%f", fParamStorage[index]);
        //mxmlSetTextf(eParam, false, "%f", fParamStorage[index]);
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
    if (elem.contains("DOCTYPE"))
    {
        if (where == MXML_WS_AFTER_CLOSE)
            return ("\n");
    }
    else if (elem.contains("preset"))
    {
        if (where == MXML_WS_AFTER_OPEN)
            return ("\n");
    }
    else if (elem.contains("preset") || elem.contains("params"))
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
