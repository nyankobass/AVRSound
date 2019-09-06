const midi_output_list = [];
var output_index = 0;
const midi_input_list = [];

const slider_property = [
    {text: "Expression", number: 11, init: 2, type: "general", max: 3},
    {text: "Attack Time", number: 73, init: 0, type: "general", max: 15},
    {text: "Decay Time", number: 75, init: 0, type: "general", max: 7},
    {text: "Sustain Level", number: 7, init: 15, type: "general", max: 15},
    {text: "Release Time", number: 72, init: 0, type: "general", max: 7},
    {text: "Vibrato Rate", number: 76, init: 0, type: "vibrato", max: 127},
    {text: "Vibrato Depth", number: 77, init: 0, type: "vibrato", max: 15},
    {text: "Vibrato Delay", number: 78, init: 0, type: "vibrato", max: 127},
    {text: "Reverb Send", number: 91, init: 0, type: "reverb", max: 15},
    {text: "Reverb Time", number: 12, init: 0, type: "reverb", max: 7},
]

/* 起動時実行 */
window.onload = function () {
    navigator.requestMIDIAccess({ sysex: false }).then(successCallback, errorCallback);
}


/* ==================================================================== */
/* スライダー 関連 */
/* ==================================================================== */
function sliderInit() {
    var slider_box = {
        "general": document.querySelector("#general-slider"),
        "vibrato": document.querySelector("#vibrato-slider"),
        "reverb": document.querySelector("#reverb-slider"),
    }

    for (var index = 0; index < slider_property.length; index++){
        const data = slider_property[index];

        const div = document.createElement('div');
        div.className = 'slider-box';

        const title_div = document.createElement('div');
        title_div.className = 'slider-title';
        title_div.innerText = data.text  + " : " + String(data.init);

        /* スライダー */
        const slider_div = document.createElement('div');
        slider_div.className = 'slider';

        const slider = document.createElement('input');
        slider.type = "range";
        slider.className = "input-range";
        slider.min = "0";
        slider.max = String(data.max);
        slider.defaultValue = String(data.init);

        slider_div.appendChild(slider);

        div.appendChild(title_div);
        div.appendChild(slider_div);

        slider_box[data.type].appendChild(div);

        /* Slider変化時コールバック設定 */
        slider.addEventListener("change", function () {
            const send_value = Number(this.value) * (128 / (Number(this.max) + 1))
            onSliderChange(data.number, send_value);

            title_div.innerText = data.text + " : " + String(send_value);
        }, false);
    }

    sendAllValue();
}

function onSliderChange(number, value){
    const output_device = midi_output_list[output_index];
    output_device.send([0xB0, number, value]);
}


function sendAllValue(){
    const qs1 = document.querySelectorAll(".input-range");

	qs1.forEach(function(a) {
        var evt = document.createEvent( "Event" ); // マウスイベント作成
        evt.initEvent( "change", false, false ); // イベントの設定
        a.dispatchEvent( evt ); // イベントを強制的に発生
	});
}






/* ==================================================================== */
/* MIDI 関連 */
/* ==================================================================== */
/* midi 初期化 */
function successCallback(midi) {
    alert("MIDI ready!");	//MIDIが使える時に表示される。

    if (typeof midi.inputs === "function") {
        midi_input_list = midi.inputs();
        midi_output_list = midi.outputs();
    } else {
        var inputIterator = midi.inputs.values();
        for (var o = inputIterator.next(); !o.done; o = inputIterator.next()) {
            midi_input_list.push(o.value);
        }
        var outputIterator = midi.outputs.values();
        for (var o = outputIterator.next(); !o.done; o = outputIterator.next()) {
            midi_output_list.push(o.value);
        }
    }

    /* Device一覧更新 */
    inputSelectListUpdate();
    outputSelectListUpdate();

    /* スライダーの初期化 */
    sliderInit();
}

function errorCallback(msg) {
    alert("Failed to get MIDI access - " + msg);	//MIDIが使えない時に表示される。
}

/* Device一覧更新 */
function outputSelectListUpdate() {
    var output_select = document.querySelector("#midi-output");
    for (var i = 0; i < midi_output_list.length; i++) {
        output_select.appendChild((new Option(midi_output_list[i]["name"], i)));
    }

    output_select.addEventListener("change", function () {
        output_index = this.selectedIndex;
        sendAllValue();
    }, false);
}

/* Device一覧更新 */
function inputSelectListUpdate() {
    var input_select = document.querySelector("#midi-input");
    for (var i = 0; i < midi_input_list.length; i++) {
        input_select.appendChild((new Option(midi_input_list[i]["name"], i)));
    }

    midi_input_list[0].addEventListener('midimessage', inputEvent, false);

    input_select.addEventListener("change", function () {
        /* イベントリスナー解除 */
        for (var i = 0; i < midi_input_list.length; i++) {
            midi_input_list[i].removeEventListener('midimessage', inputEvent, false);
        }

        /* 新規登録 */
        const index = this.selectedIndex;
        midi_input_list[index].addEventListener('midimessage', inputEvent, false);
    }, false);
}

/* インプットイベント */
function inputEvent(event){
    if ((event.data[0] & 0xF0) === 0xB0){
        return;
    }

    /* インプットの内容をアウトプットにスルーする。 */
    const output_device = midi_output_list[output_index];
    output_device.send(event.data);
}