# 128 x 32 
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

# Python variable to store the latest grid string
saved_grid_string = ""

@app.route("/")
def index():
    return render_template('index.html', rows=32, cols=128)

@app.route("/save", methods=["POST"])
def save():
    global saved_grid_string
    
    # Receive JSON payload from Javascript fetch
    data = request.get_json()
    saved_grid_string = data.get("grid_string", "")
    
    # Log to terminal for verification
    print(f"Saved Grid String (Length: {len(saved_grid_string)}): {saved_grid_string[:64]}...")
    
    return jsonify({
        "status": "success", 
        "length": len(saved_grid_string)
    })

if __name__ == "__main__":
    app.run(debug=True)
# HID 
# Grid ? Quick Changes

from flask import Flask, render_template

app = Flask(__name__)

@app.route("/")
def index():
    return render_template('index.html')

if __name__ == "__main__":
    app.run(debug=True)