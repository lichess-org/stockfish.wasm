const readline = require("readline");
const stockfish = require("./stockfish.js");

const runRepl = async () => {
  const sf = await stockfish();
  const rl = readline.createInterface({ input: process.stdin });
  for await (const command of rl) {
    if (command == 'quit') { break; }
    sf.postMessage(command);
  }
  sf.terminate();
};

const runOneshot = async (command) => {
  const sf = await stockfish();
  sf.postMessage(command);
}

const main = () => {
  const argv = process.argv.slice(2);
  if (argv.length == 0) {
    runRepl();
  } else {
    runOneshot(argv.join(' '));
  }
}

main();
