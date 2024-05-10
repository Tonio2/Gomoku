## Start virtualenv

```bash
source venv/bin/activate
```

**If not done already, install requirements**

```bash
pip install -r requirements.txt
```

## Compile gomoku engine lib

```bash
make lib
```

## Start flask server

```bash
cd GomokuBack
flask --app server_gomoku.py run --debug --host=0.0.0.0 --port 5000
```

## Start react server

```bash
cd GomokuFront
npm start
```

**If not done already, install requirements**

```
npm i
```