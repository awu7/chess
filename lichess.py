import berserk
import threading
import pexpect
import time

client = berserk.Client(berserk.TokenSession(open("token.txt").read().strip()))
# client.account.upgrade_to_bot()

class Game(threading.Thread):
    def __init__(self, game_id, color, **kwargs):
        super().__init__(**kwargs)
        self.game_id = game_id
        self.stream = client.bots.stream_game_state(game_id)
        self.current_state = next(self.stream)
        self.color = color

    def run(self):
        try:
            engine = pexpect.spawn("./a.out")
            engine.timeout = None
            engine.expect("Print moves only")
            engine.sendline("1")
            engine.expect("Bot colour")
            engine.sendline(str(int(self.color == "black")))
            if self.color == "white":
                engine.expect("\n")
                engine.expect("\n")
                move = engine.before.decode("utf-8").strip()
                client.bots.make_move(self.game_id, move)
            engine.expect("> ")
            for event in self.stream:
                # print(event)
                if event["type"] == "gameState":
                    if(event["moves"].count(" ") % 2 == ["white", "black"].index(self.color)):
                        continue
                    # print("PMOVE:", event["moves"].split()[-1][:4])
                    start = time.time()
                    engine.sendline(event["moves"].split()[-1][:4])
                    engine.expect("> ")
                    print("Elapsed:", time.time() - start)
                    # print(engine.before)
                    move = engine.before.decode("utf-8").split()[-1]
                    # print("MOVE:", move)
                    client.bots.make_move(self.game_id, move)
                elif event["type"] == "chatLine":
                    pass
        except Exception as e:
            print(e)
            engine.terminate(force=True)
            client.bots.resign_game(self.game_id)

for event in client.bots.stream_incoming_events():
    if event["type"] == "challenge":
        client.bots.accept_challenge(event["challenge"]["id"])
    elif event["type"] == "gameStart":
        print(event)
        game = Game(event["game"]["id"], event["game"]["color"])
        game.start()
