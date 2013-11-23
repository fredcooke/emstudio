// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    Rectangle {
        x:0
        y:0
        width: 400
        height: 300
        color: "grey"
        Text {
            x:10
            y:130
            color: "white"
            text: "Current Decoder Offset:"
        }
        TextEdit {
            id: tpstext
            x:200
            y:130
            color: "white"
        }
	Text {
		x:10
		y:150
		color: "white"
		text: "Configured Output Events"
	}
	TextEdit {
		id: configoutputeventstext
		x:200
		y:150
		color: "white"
	}
	Text {
		x:10
		y:170
		color: "white"
		text: "injections Per Cyle (Per Injector)"
	}
	TextEdit {
		id: configinjectionspercycle
		x:250
		y:170
		color: "white"
	}
        Rectangle {
            x: 10
            y: 10
            radius:5
            width:380
            height:120
            color: "white"
            Rectangle {
                x:5
                y:5
                width:370
                height:110
                radius:5
                color: "grey"
                Text {
                    x:5
                    y:5
                    width:360
                    height:100
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: "These are the scheduler settings for your engine. Current values are displayed, enter new values and then click Write Values to save them to flash."
                }
            }
        }

        Rectangle {
            x:10
            y:230
            width:200
            height:30
            radius:5
            color:"white"
            Rectangle {
                x:5
                y:5
                width:190
                height:20
                radius:5
                color: "blue"
                Text {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: "Write Values"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            console.log("Capture Clicked");
				numberOfConfiguredOutputEvents.setValue(configoutputeventstext.text);
				numberOfInjectionsPerEngineCycle.setValue(configinjectionspercycle.text);
				decoderEngineOffset.setValue(tpstext.text);
				

                        }
                    }
                }
            }
        }
    Component.onCompleted: {
        decoder.payloadDecoded.connect(decode);
	decoderEngineOffset.update.connect(offsetupdate);
	numberOfConfiguredOutputEvents.update.connect(outputeventupdate);
	numberOfInjectionsPerEngineCycle.update.connect(injectionspercycleupdate);

	tpstext.text = decoderEngineOffset.value();
	configoutputeventstext.text = numberOfConfiguredOutputEvents.value();
	configinjectionspercycle.text = numberOfInjectionsPerEngineCycle.value();

    }
	function offsetupdate() {
	console.log("Offset update");
		tpstext.text = decoderEngineOffset.value();
	}
	function outputeventupdate() {
		configoutputeventstext.text = numberOfConfiguredOutputEvents.value();
	}
	function injectionspercycleupdate() {
		configinjectionspercycle.text = numberOfInjectionsPerEngineCycle.value();
	}
    function decode(map){
    //console.log("Decoded");
//        tpstext.text = map["TPS"];
    }

}
}
