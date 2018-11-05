(function () {
    Module.locateFile = function(file) {
      return file;
    };

    Module.mainScriptUrlOrBlob = Module.locateFile('stockfish.js');

    Module.noExitRuntime = true;

    var listeners = [];

    Module.print = function(line) {
      if (listeners.length === 0) console.log(line);
      for (var i = 0; i < listeners.length; i++) listeners[i](line);
    };

    Module.addMessageListener = function(listener) {
      listeners.push(listener);
    };

    var queue = [];

    Module.postMessage = function(command) {
      queue.push(command);
    };

    Module.postRun = function() {
      Module.postMessage = function(command) {
        Module.ccall('uci_command', 'number', ['string'], [command]);
      };

      for (var i = 0; i < queue.length; i++) Module.postMessage(queue[i]);
      queue = null;
    };
})();
