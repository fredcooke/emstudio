import Qt 4.7
import GaugeImage 0.1

Rectangle {
	x:0
	y:0
	width: 1280
	height: 800
	color: "black"

	GaugeImage {
		x:0
		y:0
		width:300
		height:300
		minimum:0
		maximum:6000
		numLabels:3
		startAngle:45
		endAngle:315
		text: "RPM"
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["RPM_DURATION"] ? propertyMap["RPM_DURATION"] : 50) } }
		m_value: (propertyMap["RPM"] ? propertyMap["RPM"] : 0)
	}
	GaugeImage {
		x:300
		y:0
		width:300
		height:300
		minimum:0
		maximum:40
		numLabels:3
		startAngle:45
		endAngle:315
		text: "Ign Timing"
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["Advance_DURATION"] ? propertyMap["Advance_DURATION"] : 50) } }
		m_value: (propertyMap["Advance"] ? propertyMap["Advance"] : 0)
	}
	GaugeImage {
		x:0
		y:300
		width:300
		height:300
		minimum:0
		maximum:50
		numLabels:5
		startAngle:45
		endAngle:315
		text: "Pulse Width"
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["EffectivePW_DURATION"] ? propertyMap["EffectivePW_DURATION"] : 50) } }
		m_value: (propertyMap["EffectivePW"] ? propertyMap["EffectivePW"] : 0)
	}
	GaugeImage {
		x:300
		y:300
		width:300
		height:300
		minimum:0
		maximum:250
		numLabels:5
		startAngle:45
		endAngle:315
		text: "Coolant"
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["CHT_DURATION"] ? propertyMap["CHT_DURATION"] : 50) } }
		m_value: (propertyMap["CHT"] ? propertyMap["CHT"] : 0)
	}
	GaugeImage {
		x:600
		y:0
		width:300
		height:300
		minimum:0
		maximum:2
		numLabels:2
		startAngle:45
		endAngle:315
		text: "O2"
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["EGO_DURATION"] ? propertyMap["EGO_DURATION"] : 50) } }
		m_value: (propertyMap["EGO"] ? propertyMap["EGO"] : 0)
	}
	GaugeImage {
		x:600
		y:300
		width:300
		height:300
		minimum:0
		maximum:600
		numLabels:6
		startAngle:45
		endAngle:315
		text: "MAP"
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["MAP_DURATION"] ? propertyMap["MAP_DURATION"] : 50) } }
		m_value: (propertyMap["MAP"] ? propertyMap["MAP"] : 0)
	}

}
