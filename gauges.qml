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
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["0105_DURATION"] ? propertyMap["0105_DURATION"] : 50) } }
		m_value: (propertyMap["0105"] ? propertyMap["0105"] : 0)
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
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["0105_DURATION"] ? propertyMap["0105_DURATION"] : 50) } }
		m_value: (propertyMap["0106"] ? propertyMap["0106"] : 0)
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
		text: "Duty Cycle"
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["0105_DURATION"] ? propertyMap["0105_DURATION"] : 50) } }
		m_value: (propertyMap["0107"] ? propertyMap["0107"] : 0)
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
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["0105_DURATION"] ? propertyMap["0105_DURATION"] : 50) } }
		m_value: (propertyMap["0108"] ? propertyMap["0108"] : 0)
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
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["0105_DURATION"] ? propertyMap["0105_DURATION"] : 50) } }
		m_value: (propertyMap["0109"] ? propertyMap["0109"] : 0)
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
		Behavior on m_value {  PropertyAnimation { properties: "m_value"; duration: (propertyMap["0105_DURATION"] ? propertyMap["0105_DURATION"] : 50) } }
		m_value: (propertyMap["0110"] ? propertyMap["0110"] : 0)
	}

}