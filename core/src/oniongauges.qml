/**
 * Aviator gauge set
 *
 * Author: Ari "MrOnion" Karhu (ari@baboonplanet.com)
 */

import Qt 4.7
import AviatorGauges 0.1

Rectangle {
	x:0
	y:0
	width: 1280
	height: 900
	color: "black"

    // Big 1
    RoundGauge {
        x:10
        y:10
        width:300
        height:300
        low:0
        high:50
        title: "PW"
        property string propertyMapProperty: "EffectivePW"
        value: (propertyMap["EffectivePW"] ? propertyMap["EffectivePW"] : 0)
    }

    // Big 2
    RoundGauge {
        x:10
        y:320
        width:300
        height:300
        low: 0
        high: 2
        lowDanger: 0.4
        lowWarning: 0.8
        highWarning: 1.2
        highDanger: 1.6
        decimal: 2
        title: "EGO"
        property string propertyMapProperty:"EGO"
        value: (propertyMap["EGO"] ? propertyMap["EGO"] : 0)
	}

    // Small 1
    RoundGauge {
        x:330
        y:10
        width:200
        height:200
        low: 0
        high: 8000
        highWarning: 6000
        highDanger: 7000
        title: "RPM"
        property string propertyMapProperty:"RPM"
        value: (propertyMap["RPM"] ? propertyMap["RPM"] : 0)
	}

    // Small 2
    RoundGauge {
        x:550
        y:10
        width:200
        height:200
        low: 0
        high: 127
        title: "VE"
        property string propertyMapProperty:"VEMain"
        value: (propertyMap["VEMain"] ? propertyMap["VEMain"] : 0)
	}

    // Small 3
    RoundGauge {
        x:330
        y:210
        width:200
        height:200
        low: 0
        high: 100
        title: "TPS"
        unit: "%"
        property string propertyMapProperty:"TPS"
        value: (propertyMap["TPS"] ? propertyMap["TPS"] : 0)
    }

    // Small 4
    RoundGauge {
        x:550
        y:210
        width:200
        height:200
        low: 0
        high: 600
        title: "MAP"
        property string propertyMapProperty:"MAP"
        value: (propertyMap["MAP"] ? propertyMap["MAP"] : 0)
    }

    // Small 5
    RoundGauge {
        x:330
        y:410
        width:200
        height:200
        low: -10
        high: 50
        lowDanger: 0
        lowWarning: 10
        title: "Advance"
        unit: "deg"
        property string propertyMapProperty:"Advance"
        value: (propertyMap["Advance"] ? propertyMap["Advance"] : 0)
    }

    // Small 6
    RoundGauge {
        x:550
        y:410
        width:200
        height:200
        low: 0
        high: 10
        lowWarning: 1
        lowDanger: 0.5
        highWarning: 6
        highDanger: 8
        decimal: 2
        title: "Dwell"
        unit: "ms"
        property string propertyMapProperty:"Dwell"
        value: (propertyMap["Dwell"] ? propertyMap["Dwell"] : 0)
    }

    // Bars
    BarGauge {
        x:800
        y:10
        width:240
        height:120
        low:50
        high:150
        highWarning:100
        highDanger:120
        decimal:1
        title: "CHT"
        unit: "\u00B0C"
        property string propertyMapProperty:"CHT"
        value: (propertyMap["CHT"] ? propertyMap["CHT"] : 0)
	}

    BarGauge {
        x:800
        y:130
        width:240
        height:120
        low:-40
        high:120
        decimal: 1
        title: "IAT"
        unit: "\u00B0C"
        property string propertyMapProperty:"IAT"
        value: (propertyMap["IAT"] ? propertyMap["IAT"] : 0)
	}

    BarGauge {
        x:800
        y:250
        width:240
        height:120
        low:0
        high:20
        lowWarning:10
        lowDanger: 8
        highWarning: 15
        highDanger: 18
        decimal: 2
        title: "BRV"
        unit: "Volt"
        property string propertyMapProperty:"BRV"
        value: (propertyMap["BRV"] ? propertyMap["BRV"] : 0)
    }

    BarGauge {
        x:800
        y:370
        width:240
        height:120
        low:-100
        high:100
        title: "DTPS"
        property string propertyMapProperty:"DTPS"
        value: (propertyMap["DTPS"] ? propertyMap["DTPS"] : 0)
    }

    BarGauge {
        x:800
        y:490
        width:240
        height:120
        low:100
        high:300
        title: "ETE"
        unit: "%"
        property string propertyMapProperty:"ETE"
        value: (propertyMap["ETE"] ? propertyMap["ETE"] : 0)
    }
}
