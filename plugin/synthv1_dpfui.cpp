// synthv1_dpfui.cpp
//
/****************************************************************************
   Copyright (C) 2023-2024, AnClark Liu. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#include "synthv1_dpfui.h"

#include "DearImGui/imgui.h"
#include "DistrhoPluginInfo.h"

#include "synthv1.h"
#include "synthv1_dpf.h"


static thread_local String _test_preset_xml(R"(<!DOCTYPE synthv1>
<preset name="test_config" version="1.1.3">
 <params>
  <param index="0" name="DCO1_SHAPE1">0</param>
  <param index="1" name="DCO1_WIDTH1">0.923248</param>
  <param index="2" name="DCO1_BANDL1">0</param>
  <param index="3" name="DCO1_SYNC1">0</param>
  <param index="4" name="DCO1_SHAPE2">1</param>
  <param index="5" name="DCO1_WIDTH2">0.776556</param>
  <param index="6" name="DCO1_BANDL2">0</param>
  <param index="7" name="DCO1_SYNC2">0</param>
  <param index="8" name="DCO1_BALANCE">-0.217097</param>
  <param index="9" name="DCO1_DETUNE">0.359445</param>
  <param index="10" name="DCO1_PHASE">0.0300526</param>
  <param index="11" name="DCO1_RINGMOD">0.318483</param>
  <param index="12" name="DCO1_OCTAVE">0</param>
  <param index="13" name="DCO1_TUNING">0</param>
  <param index="14" name="DCO1_GLIDE">0.0187891</param>
  <param index="15" name="DCO1_ENVTIME">0.5</param>
  <param index="16" name="DCF1_ENABLED">1</param>
  <param index="17" name="DCF1_CUTOFF">0.552113</param>
  <param index="18" name="DCF1_RESO">0</param>
  <param index="19" name="DCF1_TYPE">0</param>
  <param index="20" name="DCF1_SLOPE">0</param>
  <param index="21" name="DCF1_ENVELOPE">0.641281</param>
  <param index="22" name="DCF1_ATTACK">0.133791</param>
  <param index="23" name="DCF1_DECAY">0.48218</param>
  <param index="24" name="DCF1_SUSTAIN">0.553406</param>
  <param index="25" name="DCF1_RELEASE">0.367374</param>
  <param index="26" name="LFO1_ENABLED">1</param>
  <param index="27" name="LFO1_SHAPE">1</param>
  <param index="28" name="LFO1_WIDTH">0.975992</param>
  <param index="29" name="LFO1_BPM">130.135</param>
  <param index="30" name="LFO1_RATE">0.495721</param>
  <param index="31" name="LFO1_SYNC">0</param>
  <param index="32" name="LFO1_SWEEP">-0.295761</param>
  <param index="33" name="LFO1_PITCH">0.242864</param>
  <param index="34" name="LFO1_BALANCE">-0.310322</param>
  <param index="35" name="LFO1_RINGMOD">0.224416</param>
  <param index="36" name="LFO1_CUTOFF">-0.113912</param>
  <param index="37" name="LFO1_RESO">-0.188117</param>
  <param index="38" name="LFO1_PANNING">-0.324569</param>
  <param index="39" name="LFO1_VOLUME">0.103032</param>
  <param index="40" name="LFO1_ATTACK">0.300625</param>
  <param index="41" name="LFO1_DECAY">0</param>
  <param index="42" name="LFO1_SUSTAIN">0.816777</param>
  <param index="43" name="LFO1_RELEASE">0.620098</param>
  <param index="44" name="DCA1_VOLUME">0.657145</param>
  <param index="45" name="DCA1_ATTACK">0.0021635</param>
  <param index="46" name="DCA1_DECAY">0</param>
  <param index="47" name="DCA1_SUSTAIN">0.850023</param>
  <param index="48" name="DCA1_RELEASE">0</param>
  <param index="49" name="OUT1_WIDTH">0</param>
  <param index="50" name="OUT1_PANNING">0</param>
  <param index="51" name="OUT1_FXSEND">1</param>
  <param index="52" name="OUT1_VOLUME">0.5</param>
  <param index="53" name="DEF1_PITCHBEND">0.2</param>
  <param index="54" name="DEF1_MODWHEEL">0.2</param>
  <param index="55" name="DEF1_PRESSURE">0.2</param>
  <param index="56" name="DEF1_VELOCITY">0.2</param>
  <param index="57" name="DEF1_CHANNEL">0</param>
  <param index="58" name="DEF1_MONO">0</param>
  <param index="59" name="DCO2_SHAPE1">1</param>
  <param index="60" name="DCO2_WIDTH1">0.786643</param>
  <param index="61" name="DCO2_BANDL1">0</param>
  <param index="62" name="DCO2_SYNC1">0</param>
  <param index="63" name="DCO2_SHAPE2">1</param>
  <param index="64" name="DCO2_WIDTH2">0.979507</param>
  <param index="65" name="DCO2_BANDL2">0</param>
  <param index="66" name="DCO2_SYNC2">0</param>
  <param index="67" name="DCO2_BALANCE">0.0469992</param>
  <param index="68" name="DCO2_DETUNE">0.160389</param>
  <param index="69" name="DCO2_PHASE">0.137944</param>
  <param index="70" name="DCO2_RINGMOD">0.16751</param>
  <param index="71" name="DCO2_OCTAVE">-2</param>
  <param index="72" name="DCO2_TUNING">0</param>
  <param index="73" name="DCO2_GLIDE">0.237218</param>
  <param index="74" name="DCO2_ENVTIME">0.5</param>
  <param index="75" name="DCF2_ENABLED">1</param>
  <param index="76" name="DCF2_CUTOFF">0.545292</param>
  <param index="77" name="DCF2_RESO">0.233626</param>
  <param index="78" name="DCF2_TYPE">2</param>
  <param index="79" name="DCF2_SLOPE">0</param>
  <param index="80" name="DCF2_ENVELOPE">0.836002</param>
  <param index="81" name="DCF2_ATTACK">0</param>
  <param index="82" name="DCF2_DECAY">0.279166</param>
  <param index="83" name="DCF2_SUSTAIN">0.466136</param>
  <param index="84" name="DCF2_RELEASE">0.677106</param>
  <param index="85" name="LFO2_ENABLED">1</param>
  <param index="86" name="LFO2_SHAPE">1</param>
  <param index="87" name="LFO2_WIDTH">0.711414</param>
  <param index="88" name="LFO2_BPM">197.524</param>
  <param index="89" name="LFO2_RATE">0.66595</param>
  <param index="90" name="LFO2_SYNC">0</param>
  <param index="91" name="LFO2_SWEEP">-0.065928</param>
  <param index="92" name="LFO2_PITCH">0.201572</param>
  <param index="93" name="LFO2_BALANCE">0.0362903</param>
  <param index="94" name="LFO2_RINGMOD">0.349946</param>
  <param index="95" name="LFO2_CUTOFF">0.126608</param>
  <param index="96" name="LFO2_RESO">0.177094</param>
  <param index="97" name="LFO2_PANNING">0.151167</param>
  <param index="98" name="LFO2_VOLUME">0.581917</param>
  <param index="99" name="LFO2_ATTACK">0</param>
  <param index="100" name="LFO2_DECAY">0.187682</param>
  <param index="101" name="LFO2_SUSTAIN">1</param>
  <param index="102" name="LFO2_RELEASE">0.311757</param>
  <param index="103" name="DCA2_VOLUME">0.317346</param>
  <param index="104" name="DCA2_ATTACK">0.00287997</param>
  <param index="105" name="DCA2_DECAY">0.0848748</param>
  <param index="106" name="DCA2_SUSTAIN">0.972756</param>
  <param index="107" name="DCA2_RELEASE">0.0749383</param>
  <param index="108" name="OUT2_WIDTH">0</param>
  <param index="109" name="OUT2_PANNING">0</param>
  <param index="110" name="OUT2_FXSEND">1</param>
  <param index="111" name="OUT2_VOLUME">0.5</param>
  <param index="112" name="DEF2_PITCHBEND">0.2</param>
  <param index="113" name="DEF2_MODWHEEL">0.2</param>
  <param index="114" name="DEF2_PRESSURE">0.2</param>
  <param index="115" name="DEF2_VELOCITY">0.2</param>
  <param index="116" name="DEF2_CHANNEL">0</param>
  <param index="117" name="DEF2_MONO">0</param>
  <param index="118" name="CHO1_WET">0</param>
  <param index="119" name="CHO1_DELAY">0.5</param>
  <param index="120" name="CHO1_FEEDB">0.5</param>
  <param index="121" name="CHO1_RATE">0.5</param>
  <param index="122" name="CHO1_MOD">0.5</param>
  <param index="123" name="FLA1_WET">0</param>
  <param index="124" name="FLA1_DELAY">0.5</param>
  <param index="125" name="FLA1_FEEDB">0.5</param>
  <param index="126" name="FLA1_DAFT">0</param>
  <param index="127" name="PHA1_WET">0</param>
  <param index="128" name="PHA1_RATE">0.5</param>
  <param index="129" name="PHA1_FEEDB">0.5</param>
  <param index="130" name="PHA1_DEPTH">0.5</param>
  <param index="131" name="PHA1_DAFT">0</param>
  <param index="132" name="DEL1_WET">0</param>
  <param index="133" name="DEL1_DELAY">0.5</param>
  <param index="134" name="DEL1_FEEDB">0.5</param>
  <param index="135" name="DEL1_BPM">180</param>
  <param index="136" name="REV1_WET">0</param>
  <param index="137" name="REV1_ROOM">0.5</param>
  <param index="138" name="REV1_DAMP">0.5</param>
  <param index="139" name="REV1_FEEDB">0.5</param>
  <param index="140" name="REV1_WIDTH">0</param>
  <param index="141" name="DYN1_COMPRESS">0</param>
  <param index="142" name="DYN1_LIMITER">1</param>
  <param index="143" name="KEY1_LOW">0</param>
  <param index="144" name="KEY1_HIGH">127</param>
 </params>
</preset>
)");

static thread_local String _test_bank_xml(R"(<!DOCTYPE synthv1>
<bank name="TEstBank" author="AnClark Liu">
<description>For testing my bank.</description>
<copyright>MIT</copyright>
<preset name="test_config_1" version="1.1.3">
 <params>
  <param index="0" name="DCO1_SHAPE1">0</param>
  <param index="1" name="DCO1_WIDTH1">0.923248</param>
  <param index="2" name="DCO1_BANDL1">0</param>
  <param index="3" name="DCO1_SYNC1">0</param>
  <param index="4" name="DCO1_SHAPE2">1</param>
  <param index="5" name="DCO1_WIDTH2">0.776556</param>
  <param index="6" name="DCO1_BANDL2">0</param>
  <param index="7" name="DCO1_SYNC2">0</param>
  <param index="8" name="DCO1_BALANCE">-0.217097</param>
  <param index="9" name="DCO1_DETUNE">0.359445</param>
  <param index="10" name="DCO1_PHASE">0.0300526</param>
  <param index="11" name="DCO1_RINGMOD">0.318483</param>
  <param index="12" name="DCO1_OCTAVE">0</param>
  <param index="13" name="DCO1_TUNING">0</param>
  <param index="14" name="DCO1_GLIDE">0.0187891</param>
  <param index="15" name="DCO1_ENVTIME">0.5</param>
  <param index="16" name="DCF1_ENABLED">1</param>
  <param index="17" name="DCF1_CUTOFF">0.552113</param>
  <param index="18" name="DCF1_RESO">0</param>
  <param index="19" name="DCF1_TYPE">0</param>
  <param index="20" name="DCF1_SLOPE">0</param>
  <param index="21" name="DCF1_ENVELOPE">0.641281</param>
  <param index="22" name="DCF1_ATTACK">0.133791</param>
  <param index="23" name="DCF1_DECAY">0.48218</param>
  <param index="24" name="DCF1_SUSTAIN">0.553406</param>
  <param index="25" name="DCF1_RELEASE">0.367374</param>
  <param index="26" name="LFO1_ENABLED">1</param>
  <param index="27" name="LFO1_SHAPE">1</param>
  <param index="28" name="LFO1_WIDTH">0.975992</param>
  <param index="29" name="LFO1_BPM">130.135</param>
  <param index="30" name="LFO1_RATE">0.495721</param>
  <param index="31" name="LFO1_SYNC">0</param>
  <param index="32" name="LFO1_SWEEP">-0.295761</param>
  <param index="33" name="LFO1_PITCH">0.242864</param>
  <param index="34" name="LFO1_BALANCE">-0.310322</param>
  <param index="35" name="LFO1_RINGMOD">0.224416</param>
  <param index="36" name="LFO1_CUTOFF">-0.113912</param>
  <param index="37" name="LFO1_RESO">-0.188117</param>
  <param index="38" name="LFO1_PANNING">-0.324569</param>
  <param index="39" name="LFO1_VOLUME">0.103032</param>
  <param index="40" name="LFO1_ATTACK">0.300625</param>
  <param index="41" name="LFO1_DECAY">0</param>
  <param index="42" name="LFO1_SUSTAIN">0.816777</param>
  <param index="43" name="LFO1_RELEASE">0.620098</param>
  <param index="44" name="DCA1_VOLUME">0.657145</param>
  <param index="45" name="DCA1_ATTACK">0.0021635</param>
  <param index="46" name="DCA1_DECAY">0</param>
  <param index="47" name="DCA1_SUSTAIN">0.850023</param>
  <param index="48" name="DCA1_RELEASE">0</param>
  <param index="49" name="OUT1_WIDTH">0</param>
  <param index="50" name="OUT1_PANNING">0</param>
  <param index="51" name="OUT1_FXSEND">1</param>
  <param index="52" name="OUT1_VOLUME">0.5</param>
  <param index="53" name="DEF1_PITCHBEND">0.2</param>
  <param index="54" name="DEF1_MODWHEEL">0.2</param>
  <param index="55" name="DEF1_PRESSURE">0.2</param>
  <param index="56" name="DEF1_VELOCITY">0.2</param>
  <param index="57" name="DEF1_CHANNEL">0</param>
  <param index="58" name="DEF1_MONO">0</param>
  <param index="59" name="DCO2_SHAPE1">1</param>
  <param index="60" name="DCO2_WIDTH1">0.786643</param>
  <param index="61" name="DCO2_BANDL1">0</param>
  <param index="62" name="DCO2_SYNC1">0</param>
  <param index="63" name="DCO2_SHAPE2">1</param>
  <param index="64" name="DCO2_WIDTH2">0.979507</param>
  <param index="65" name="DCO2_BANDL2">0</param>
  <param index="66" name="DCO2_SYNC2">0</param>
  <param index="67" name="DCO2_BALANCE">0.0469992</param>
  <param index="68" name="DCO2_DETUNE">0.160389</param>
  <param index="69" name="DCO2_PHASE">0.137944</param>
  <param index="70" name="DCO2_RINGMOD">0.16751</param>
  <param index="71" name="DCO2_OCTAVE">-2</param>
  <param index="72" name="DCO2_TUNING">0</param>
  <param index="73" name="DCO2_GLIDE">0.237218</param>
  <param index="74" name="DCO2_ENVTIME">0.5</param>
  <param index="75" name="DCF2_ENABLED">1</param>
  <param index="76" name="DCF2_CUTOFF">0.545292</param>
  <param index="77" name="DCF2_RESO">0.233626</param>
  <param index="78" name="DCF2_TYPE">2</param>
  <param index="79" name="DCF2_SLOPE">0</param>
  <param index="80" name="DCF2_ENVELOPE">0.836002</param>
  <param index="81" name="DCF2_ATTACK">0</param>
  <param index="82" name="DCF2_DECAY">0.279166</param>
  <param index="83" name="DCF2_SUSTAIN">0.466136</param>
  <param index="84" name="DCF2_RELEASE">0.677106</param>
  <param index="85" name="LFO2_ENABLED">1</param>
  <param index="86" name="LFO2_SHAPE">1</param>
  <param index="87" name="LFO2_WIDTH">0.711414</param>
  <param index="88" name="LFO2_BPM">197.524</param>
  <param index="89" name="LFO2_RATE">0.66595</param>
  <param index="90" name="LFO2_SYNC">0</param>
  <param index="91" name="LFO2_SWEEP">-0.065928</param>
  <param index="92" name="LFO2_PITCH">0.201572</param>
  <param index="93" name="LFO2_BALANCE">0.0362903</param>
  <param index="94" name="LFO2_RINGMOD">0.349946</param>
  <param index="95" name="LFO2_CUTOFF">0.126608</param>
  <param index="96" name="LFO2_RESO">0.177094</param>
  <param index="97" name="LFO2_PANNING">0.151167</param>
  <param index="98" name="LFO2_VOLUME">0.581917</param>
  <param index="99" name="LFO2_ATTACK">0</param>
  <param index="100" name="LFO2_DECAY">0.187682</param>
  <param index="101" name="LFO2_SUSTAIN">1</param>
  <param index="102" name="LFO2_RELEASE">0.311757</param>
  <param index="103" name="DCA2_VOLUME">0.317346</param>
  <param index="104" name="DCA2_ATTACK">0.00287997</param>
  <param index="105" name="DCA2_DECAY">0.0848748</param>
  <param index="106" name="DCA2_SUSTAIN">0.972756</param>
  <param index="107" name="DCA2_RELEASE">0.0749383</param>
  <param index="108" name="OUT2_WIDTH">0</param>
  <param index="109" name="OUT2_PANNING">0</param>
  <param index="110" name="OUT2_FXSEND">1</param>
  <param index="111" name="OUT2_VOLUME">0.5</param>
  <param index="112" name="DEF2_PITCHBEND">0.2</param>
  <param index="113" name="DEF2_MODWHEEL">0.2</param>
  <param index="114" name="DEF2_PRESSURE">0.2</param>
  <param index="115" name="DEF2_VELOCITY">0.2</param>
  <param index="116" name="DEF2_CHANNEL">0</param>
  <param index="117" name="DEF2_MONO">0</param>
  <param index="118" name="CHO1_WET">0</param>
  <param index="119" name="CHO1_DELAY">0.5</param>
  <param index="120" name="CHO1_FEEDB">0.5</param>
  <param index="121" name="CHO1_RATE">0.5</param>
  <param index="122" name="CHO1_MOD">0.5</param>
  <param index="123" name="FLA1_WET">0</param>
  <param index="124" name="FLA1_DELAY">0.5</param>
  <param index="125" name="FLA1_FEEDB">0.5</param>
  <param index="126" name="FLA1_DAFT">0</param>
  <param index="127" name="PHA1_WET">0</param>
  <param index="128" name="PHA1_RATE">0.5</param>
  <param index="129" name="PHA1_FEEDB">0.5</param>
  <param index="130" name="PHA1_DEPTH">0.5</param>
  <param index="131" name="PHA1_DAFT">0</param>
  <param index="132" name="DEL1_WET">0</param>
  <param index="133" name="DEL1_DELAY">0.5</param>
  <param index="134" name="DEL1_FEEDB">0.5</param>
  <param index="135" name="DEL1_BPM">180</param>
  <param index="136" name="REV1_WET">0</param>
  <param index="137" name="REV1_ROOM">0.5</param>
  <param index="138" name="REV1_DAMP">0.5</param>
  <param index="139" name="REV1_FEEDB">0.5</param>
  <param index="140" name="REV1_WIDTH">0</param>
  <param index="141" name="DYN1_COMPRESS">0</param>
  <param index="142" name="DYN1_LIMITER">1</param>
  <param index="143" name="KEY1_LOW">0</param>
  <param index="144" name="KEY1_HIGH">127</param>
 </params>
</preset>
<preset name="test_config_2" version="1.1.3">
 <params>
  <param index="0" name="DCO1_SHAPE1">3</param>
  <param index="1" name="DCO1_WIDTH1">1</param>
  <param index="2" name="DCO1_BANDL1">1</param>
  <param index="3" name="DCO1_SYNC1">0</param>
  <param index="4" name="DCO1_SHAPE2">4</param>
  <param index="5" name="DCO1_WIDTH2">0.905149</param>
  <param index="6" name="DCO1_BANDL2">0</param>
  <param index="7" name="DCO1_SYNC2">0</param>
  <param index="8" name="DCO1_BALANCE">-0.180961</param>
  <param index="9" name="DCO1_DETUNE">0.476035</param>
  <param index="10" name="DCO1_PHASE">0.232</param>
  <param index="11" name="DCO1_RINGMOD">0.255313</param>
  <param index="12" name="DCO1_OCTAVE">0</param>
  <param index="13" name="DCO1_TUNING">0</param>
  <param index="14" name="DCO1_GLIDE">0</param>
  <param index="15" name="DCO1_ENVTIME">0.5</param>
  <param index="16" name="DCF1_ENABLED">1</param>
  <param index="17" name="DCF1_CUTOFF">0.804738</param>
  <param index="18" name="DCF1_RESO">0.75</param>
  <param index="19" name="DCF1_TYPE">3</param>
  <param index="20" name="DCF1_SLOPE">0</param>
  <param index="21" name="DCF1_ENVELOPE">0.905227</param>
  <param index="22" name="DCF1_ATTACK">0.92</param>
  <param index="23" name="DCF1_DECAY">0.349025</param>
  <param index="24" name="DCF1_SUSTAIN">0.733199</param>
  <param index="25" name="DCF1_RELEASE">0.416046</param>
  <param index="26" name="LFO1_ENABLED">1</param>
  <param index="27" name="LFO1_SHAPE">4</param>
  <param index="28" name="LFO1_WIDTH">0.994104</param>
  <param index="29" name="LFO1_BPM">62.9417</param>
  <param index="30" name="LFO1_RATE">0.606965</param>
  <param index="31" name="LFO1_SYNC">0</param>
  <param index="32" name="LFO1_SWEEP">-0.26398</param>
  <param index="33" name="LFO1_PITCH">0.131029</param>
  <param index="34" name="LFO1_BALANCE">-0.646913</param>
  <param index="35" name="LFO1_RINGMOD">0.289195</param>
  <param index="36" name="LFO1_CUTOFF">0.193674</param>
  <param index="37" name="LFO1_RESO">-0.0231769</param>
  <param index="38" name="LFO1_PANNING">-0.552533</param>
  <param index="39" name="LFO1_VOLUME">0.293263</param>
  <param index="40" name="LFO1_ATTACK">0.31552</param>
  <param index="41" name="LFO1_DECAY">0.0172563</param>
  <param index="42" name="LFO1_SUSTAIN">0.837382</param>
  <param index="43" name="LFO1_RELEASE">0.491829</param>
  <param index="44" name="DCA1_VOLUME">0.491944</param>
  <param index="45" name="DCA1_ATTACK">1</param>
  <param index="46" name="DCA1_DECAY">0.82</param>
  <param index="47" name="DCA1_SUSTAIN">0.786525</param>
  <param index="48" name="DCA1_RELEASE">0.147556</param>
  <param index="49" name="OUT1_WIDTH">0</param>
  <param index="50" name="OUT1_PANNING">0</param>
  <param index="51" name="OUT1_FXSEND">1</param>
  <param index="52" name="OUT1_VOLUME">0.5</param>
  <param index="53" name="DEF1_PITCHBEND">0.2</param>
  <param index="54" name="DEF1_MODWHEEL">0.2</param>
  <param index="55" name="DEF1_PRESSURE">0.2</param>
  <param index="56" name="DEF1_VELOCITY">0.2</param>
  <param index="57" name="DEF1_CHANNEL">0</param>
  <param index="58" name="DEF1_MONO">0</param>
  <param index="59" name="DCO2_SHAPE1">2</param>
  <param index="60" name="DCO2_WIDTH1">0.89049</param>
  <param index="61" name="DCO2_BANDL1">0</param>
  <param index="62" name="DCO2_SYNC1">0</param>
  <param index="63" name="DCO2_SHAPE2">1</param>
  <param index="64" name="DCO2_WIDTH2">0.877617</param>
  <param index="65" name="DCO2_BANDL2">0</param>
  <param index="66" name="DCO2_SYNC2">0</param>
  <param index="67" name="DCO2_BALANCE">-0.123422</param>
  <param index="68" name="DCO2_DETUNE">0.105535</param>
  <param index="69" name="DCO2_PHASE">0.269417</param>
  <param index="70" name="DCO2_RINGMOD">0.131709</param>
  <param index="71" name="DCO2_OCTAVE">-2</param>
  <param index="72" name="DCO2_TUNING">0</param>
  <param index="73" name="DCO2_GLIDE">0.318024</param>
  <param index="74" name="DCO2_ENVTIME">0.5</param>
  <param index="75" name="DCF2_ENABLED">1</param>
  <param index="76" name="DCF2_CUTOFF">0.667048</param>
  <param index="77" name="DCF2_RESO">0.167395</param>
  <param index="78" name="DCF2_TYPE">0</param>
  <param index="79" name="DCF2_SLOPE">0</param>
  <param index="80" name="DCF2_ENVELOPE">0.83015</param>
  <param index="81" name="DCF2_ATTACK">0</param>
  <param index="82" name="DCF2_DECAY">0.104165</param>
  <param index="83" name="DCF2_SUSTAIN">0.488937</param>
  <param index="84" name="DCF2_RELEASE">0.81129</param>
  <param index="85" name="LFO2_ENABLED">1</param>
  <param index="86" name="LFO2_SHAPE">3</param>
  <param index="87" name="LFO2_WIDTH">0.577796</param>
  <param index="88" name="LFO2_BPM">68.706</param>
  <param index="89" name="LFO2_RATE">0.450608</param>
  <param index="90" name="LFO2_SYNC">1</param>
  <param index="91" name="LFO2_SWEEP">-0.608422</param>
  <param index="92" name="LFO2_PITCH">-0.129187</param>
  <param index="93" name="LFO2_BALANCE">-0.131376</param>
  <param index="94" name="LFO2_RINGMOD">0.538395</param>
  <param index="95" name="LFO2_CUTOFF">0.0460913</param>
  <param index="96" name="LFO2_RESO">0.676416</param>
  <param index="97" name="LFO2_PANNING">0.335292</param>
  <param index="98" name="LFO2_VOLUME">0.817286</param>
  <param index="99" name="LFO2_ATTACK">0.000208407</param>
  <param index="100" name="LFO2_DECAY">0.114504</param>
  <param index="101" name="LFO2_SUSTAIN">0.839912</param>
  <param index="102" name="LFO2_RELEASE">0.146705</param>
  <param index="103" name="DCA2_VOLUME">0.400755</param>
  <param index="104" name="DCA2_ATTACK">0</param>
  <param index="105" name="DCA2_DECAY">0.18793</param>
  <param index="106" name="DCA2_SUSTAIN">1</param>
  <param index="107" name="DCA2_RELEASE">0.0428807</param>
  <param index="108" name="OUT2_WIDTH">0</param>
  <param index="109" name="OUT2_PANNING">0</param>
  <param index="110" name="OUT2_FXSEND">1</param>
  <param index="111" name="OUT2_VOLUME">0.5</param>
  <param index="112" name="DEF2_PITCHBEND">0.2</param>
  <param index="113" name="DEF2_MODWHEEL">0.2</param>
  <param index="114" name="DEF2_PRESSURE">0.2</param>
  <param index="115" name="DEF2_VELOCITY">0.2</param>
  <param index="116" name="DEF2_CHANNEL">0</param>
  <param index="117" name="DEF2_MONO">0</param>
  <param index="118" name="CHO1_WET">0</param>
  <param index="119" name="CHO1_DELAY">0.5</param>
  <param index="120" name="CHO1_FEEDB">0.5</param>
  <param index="121" name="CHO1_RATE">0.5</param>
  <param index="122" name="CHO1_MOD">0.5</param>
  <param index="123" name="FLA1_WET">0</param>
  <param index="124" name="FLA1_DELAY">0.5</param>
  <param index="125" name="FLA1_FEEDB">0.5</param>
  <param index="126" name="FLA1_DAFT">0</param>
  <param index="127" name="PHA1_WET">0</param>
  <param index="128" name="PHA1_RATE">0.5</param>
  <param index="129" name="PHA1_FEEDB">0.5</param>
  <param index="130" name="PHA1_DEPTH">0.5</param>
  <param index="131" name="PHA1_DAFT">0</param>
  <param index="132" name="DEL1_WET">0</param>
  <param index="133" name="DEL1_DELAY">0.5</param>
  <param index="134" name="DEL1_FEEDB">0.5</param>
  <param index="135" name="DEL1_BPM">180</param>
  <param index="136" name="REV1_WET">0</param>
  <param index="137" name="REV1_ROOM">0.5</param>
  <param index="138" name="REV1_DAMP">0.5</param>
  <param index="139" name="REV1_FEEDB">0.5</param>
  <param index="140" name="REV1_WIDTH">0</param>
  <param index="141" name="DYN1_COMPRESS">0</param>
  <param index="142" name="DYN1_LIMITER">1</param>
  <param index="143" name="KEY1_LOW">0</param>
  <param index="144" name="KEY1_HIGH">127</param>
 </params>
 <tuning enabled="1">
  <ref-pitch>440</ref-pitch>
  <ref-note>69</ref-note>
 </tuning>
</preset>
</bank>
)");

thread_local static synthv1_preset_bank my_test_bank;

//-------------------------------------------------------------------------
// SynthV1PluginUI - DPF Plugin UI interface.
//

START_NAMESPACE_DISTRHO

SynthV1PluginUI::SynthV1PluginUI()
    : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true),
      fResizeHandle(this)
{
    setGeometryConstraints(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true);

    // hide handle if UI is resizable
    if (isResizable())
        fResizeHandle.hide();

    // Set default params
    fCurrentView = View::Synth1;    // Default UI view

    // Obtain plugin config dir
    fConfigDir = get_system_config_dir() + "/synthv1";
    d_stderr("Config dir: %s", fConfigDir.buffer());

    // Initialize preset manager
    fPresetManager = std::make_unique<synthv1_preset_manager>(this);
}

// ----------------------------------------------------------------------------------------------------------------
// DSP/Plugin Callbacks

void SynthV1PluginUI::parameterChanged(uint32_t index, float value)
{
    DISTRHO_SAFE_ASSERT_RETURN(index >= 0 && index < synthv1::NUM_PARAMS, )

    fParamStorage[index] = value;

    repaint();
}

// ----------------------------------------------------------------------------------------------------------------
// Widget Callbacks

void SynthV1PluginUI::onImGuiDisplay()
{
    const float width = getWidth();
    const float height = getHeight();
    const float margin = 20.0f * getScaleFactor();

    static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

    // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
    // Based on your use case you may want one or the other.
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    if (ImGui::Begin(DISTRHO_PLUGIN_NAME, nullptr, flags))
    {
        [[maybe_unused]] constexpr uint32_t PADDING_L_R = 20;

        static constexpr ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        
        ImGui::BeginGroup();
        {
            if (ImGui::Button("Panic"))
            {
                _panic();
            }
            ImGui::SameLine(0, 40);

            if (ImGui::Button("Random"))
            {
                _randomParams();
            }
            ImGui::SameLine(0, 40);

            if (ImGui::Button("Test Loading Preset"))
            {
                fPresetManager->loadPreset(_test_preset_xml);
            }
            ImGui::SameLine(0, 10);

            if (ImGui::Button("Test Saving Preset"))
            {
                _test_preset_xml = fPresetManager->savePreset("test_preset");
                d_stderr("%s", _test_preset_xml.buffer());
            }
            ImGui::SameLine(0, 40);
            
            if (ImGui::Button("Test Loading bank"))
            {
                fPresetManager->loadBank(_test_bank_xml.buffer(), my_test_bank);

                d_stderr("Loaded bank: %s\n\tAuthor: %s\n\tDescription: %s\n\tCopyright: %s", my_test_bank.name.buffer(), my_test_bank.author.buffer(), my_test_bank.description.buffer(), my_test_bank.copyright.buffer());
                d_stderr("\tPreset count: %d", my_test_bank.presets.size());

                for (auto preset_item : my_test_bank.presets)
                {
                    d_stderr("\t\t- Name: %s", preset_item.name.buffer());
                    //d_stderr("\t\t- Content: %s", preset_item.presetXml.buffer());
                }
            }
            ImGui::SameLine(0, 10);

            if (ImGui::Button("Test Saving Bank"))
            {
                _test_bank_xml = fPresetManager->saveBank(my_test_bank);
                d_stderr("%s", _test_bank_xml.buffer());
            }
            ImGui::SameLine(0, 40);

            _addViewSwitchButton("Synth 1", View::Synth1); ImGui::SameLine();
            _addViewSwitchButton("Synth 2", View::Synth2); ImGui::SameLine();
            _addViewSwitchButton("Effects", View::Effects);
        }
        ImGui::EndGroup();

        ImGui::Separator();

        switch (fCurrentView)
        {
            case View::Synth1:
                _uiView_Synth(true);    // true means Synth 1
                break;
            
            case View::Synth2:
                _uiView_Synth(false);   // false means Synth 2
                break;
            
            case View::Effects:
                _uiView_Effects();
                break;
        }
    }
    ImGui::End();
}

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
	return new SynthV1PluginUI();
}

END_NAMESPACE_DISTRHO


//-------------------------------------------------------------------------
// synthv1_dpfui - Synth engine accessor: impl.
//

synthv1_dpfui::synthv1_dpfui(synthv1_dpf *pSynth, DISTRHO::SynthV1PluginUI *pluginUiInterface)
	: synthv1_ui(pSynth, true), m_plugin_ui(pluginUiInterface)
{
}

void synthv1_dpfui::write_function(synthv1::ParamIndex index, float fValue) const
{
	m_plugin_ui->setParameterValue(index, fValue);
}

// end of synthv1_dpfui.cpp
