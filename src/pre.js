(function () {
    Module.locateFile = function(file) {
      return file;
    };

    Module.mainScriptUrlOrBlob = Module.locateFile('stockfish.js');

    Module.print = function(line) {
      console.log('>>', line);
    };

    Module.noExitRuntime = true;

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
