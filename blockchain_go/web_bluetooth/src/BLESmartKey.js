/*
============================================================
BLESmartKey.js
============================================================
Web Bluetooth API Wrapper Library

Copyright 2017 JellyWare Inc.
Copyright 2019 Naoto Fujihiro
http://jellyware.jp/
This software is released under the MIT License.

Web Bluetooth API
https://webbluetoothcg.github.io/web-bluetooth/
*/

let BLESmartKey = function() {
    this.bluetoothDevice = null;
    this.dataCharacteristic = null;
    this.hashedUuid = {};
    this.hashedUuidLastConnected = null;

    this.onScan = function(deviceName)      {console.log("onScan");};
    this.onConnectGATT = function(uuid)     {console.log("onconnectGATT");};
    this.onRead = function(data, uuid)      {console.log("onRead");};
    this.onWrite = function(uuid)           {console.log("onWrite");};
    this.onStartNotify = function(uuid)     {console.log("onStartNotify");};
    this.onStopNotify = function(uuid)      {console.log("onStopNotify");};
    this.onDisconnect = function()          {console.log("onDisconnect");};
    this.onClear = function()               {console.log("onClear");};
    this.onReset = function()               {console.log("onReset");};
    this.onError = function(error)          {console.log("onError");};
};

BLESmartKey.prototype.setUuid = function(name, serviceUuid, characteristicUuid) {
    console.log('Execute : setUUID');

    this.hashedUuid[name] = {
        'serviceUuid': serviceUuid,
        'characteristicUuid': characteristicUuid
    };

    console.log(this.hashedUuid)
};

BLESmartKey.prototype.scan = function(uuid) {
    return (this.bluetoothDevice ? Promise.resolve() : this.requestDevice(uuid))
        .catch(error => {
            console.error('Error: ' + error);
            this.onError(error);
        })
};

BLESmartKey.prototype.requestDevice = function(uuid) {
    console.log('Execute : requestDevice');

    return navigator.bluetooth.requestDevice({
        acceptAllDevices: true,
        optionalServices: [this.hashedUuid[uuid].serviceUuid]})
        .then(device => {
            this.bluetoothDevice = device;
            this.bluetoothDevice.addEventListener('gattserverdisconnected', this.onDisconnect);
            this.onScan(this.bluetoothDevice.name);
        });
};

BLESmartKey.prototype.connectGATT = function(uuid) {
    if (!this.bluetoothDevice) {
        let error = "No Bluetooth Device";
        console.error('Error: '+ error);
        this.onError(error);
        return;
    }
    if (this.bluetoothDevice.gatt.connected && this.dataCharacteristic) {
        if (this.hashedUuidLastConnected == uuid)
            return Promise.resolve();
    }
    this.hashedUuidLastConnected = uuid;

    console.log('Execute : connect');
    return this.bluetoothDevice.gatt.connect()
        .then(server => {
            console.log('Execute : getPrimaryService');
            return server.getPrimaryService(this.hashedUuid[uuid].serviceUuid);
        })
        .then(service => {
            console.log('Execute : getCharacteristic');
            return service.getCharacteristic(this.hashedUuid[uuid].characteristicUuid);
        })
        .then(characteristic => {
            this.dataCharacteristic = characteristic;
            // this.dataCharacteristic.addEventListener('characteristicvaluechanged', this.dataChanged(this, uuid));
            this.onConnectGATT(uuid);
        })
        .catch(error => {
            console.error('Error: ' + error);
            this.onError(error);
        });
};

BLESmartKey.prototype.dataChanged = function(self, uuid) {
    return function(event) {
        self.onRead(event.target.value, uuid)
    }
};

BLESmartKey.prototype.read = function(uuid) {
    return (this.scan(uuid))
        .then( () => {
            return this.connectGATT(uuid);
        })
        .then( () => {
            console.log('Execute : readValue');
            return this.dataCharacteristic.readValue();
        })
        .catch(error => {
            console.error('Error: ' + error);
            this.onError(error);
        })
};

BLESmartKey.prototype.write = function(uuid, array_value) {
    return (this.scan(uuid))
        .then( () => {
            return this.connectGATT(uuid);
        })
        .then( () => {
            console.log('Execute : writeValue');
            data = Uint8Array.from(array_value);
            return this.dataCharacteristic.writeValue(data);
        })
        .then( () => {
            this.onWrite(uuid);
        })
        .catch(error => {
            console.error('Error: ' + error);
            this.onError(error);
        })
};

BLESmartKey.prototype.startNotify = function(uuid) {
    return (this.scan(uuid))
        .then( () => {
            return this.connectGATT();
        })
        .then( () => {
            console.log('Execute : startNotifications');
            this.dataCharacteristic.startNotifications()
        })
        .then( () => {
            this.onStartNotify(uuid);
        })
        .catch(error => {
            console.error('Error: ' + error);
            this.onError(error);
        })
};

BLESmartKey.prototype.stopNotify = function(uuid) {
    return (this.scan(uuid))
        .then( () => {
            return this.connectGATT(uuid);
        })
        .then( () => {
            console.log('Execute : stopNotifications');
            this.dataCharacteristic.stopNotifications()
        })
        .then( () => {
            this.onStopNotify();
        })
        .catch(error => {
            console.error('Error: ' + error);
            this.onError(error);
        })
};

BLESmartKey.prototype.disconnect = function() {
    if (!this.bluetoothDevice) {
        let error = "No Bluetooth Device";
        console.error('Error: ' + error);
        this.onError(error);
        return;
    }

    if (this.bluetoothDevice.gatt.connected) {
        console.log('Execute : disconnect');
        this.bluetoothDevice.gatt.disconnect();
    } else {
        let error = "Bluetooth Device is already disconnected";
        console.error('Error: ' + error);
        this.onError(error);
        return;
    }
};

BLESmartKey.prototype.clear = function() {
    console.log('Excute : Clear Device and Characteristic');
    this.bluetoothDevice = null;
    this.dataCharacteristic = null;
    this.onClear();
};

BLESmartKey.prototype.reset = function() {
    console.log('Excute : reset');
    this.disconnect();
    this.clear();
    this.onReset();
};
