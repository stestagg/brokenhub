#!/usr/bin/env python 

import flask
import json

app = flask.Flask(__name__)

@app.route("/")
def hello():
    return flask.render_template("base.html")

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5001, debug=True)
