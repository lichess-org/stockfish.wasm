(function () {
  // Message listeners

  const listeners = [];

  Module['print'] = line => {
    if (listeners.length === 0) console.log(line);
    else setTimeout(() => {
      for (const listener of listeners) listener(line);
    });
  };

  Module['addMessageListener'] = listener => {
    listeners.push(listener);
  };

  Module['removeMessageListener'] = listener => {
    const idx = listeners.indexOf(listener);
    if (idx >= 0) listeners.splice(idx, 1);
  };

  // Command queue

  const queue = [];
  let backoff = 1;

  const poll = () => {
    const command = queue.shift();
    if (command === undefined) return;

    const tryLater = Module.ccall('uci_command', 'number', ['string'], [command]);
    if (tryLater) queue.unshift(command);
    backoff = tryLater ? (backoff * 2) : 1;
    setTimeout(poll, backoff);
  };

  Module['postMessage'] = command => {
    queue.push(command);
  };

  Module['postRun'] = () => {
    Module['postMessage'] = command => {
      queue.push(command);
      if (queue.length === 1) poll();
    };
    poll();
  };
})();
