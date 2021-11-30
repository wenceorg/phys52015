import sys
import argparse
import os
import time
import yaml
import pathlib


header = """
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />

    %s
    <link rel="shortcut icon" href="/phys52015/favicon.svg" />
    <link rel="stylesheet"
          href="https://cdnjs.cloudflare.com/ajax/libs/reveal.js/4.1.2/reset.min.css"
          integrity="sha512-Mjxkx+r7O/OLQeKeIBCQ2yspG1P5muhAtv/J+p2/aPnSenciZWm5Wlnt+NOUNA4SHbnBIE/R2ic0ZBiCXdQNUg=="
          crossorigin="anonymous" referrerpolicy="no-referrer" />
    <link rel="stylesheet"
          href="https://cdnjs.cloudflare.com/ajax/libs/reveal.js/4.1.2/reveal.min.css"
          integrity="sha512-WFGU7IgfYR0dq5aORzbD+NApAXdExNZFb7LaoO8olYImBW/iZxAwjKEuT+oYcFR6gOd+DAFssq/icMn8YVbQxQ=="
          crossorigin="anonymous" referrerpolicy="no-referrer" />
    <link rel="stylesheet"
          href="https://cdnjs.cloudflare.com/ajax/libs/reveal.js/4.1.2/theme/solarized.min.css"
          integrity="sha512-sUF1FAUpi9yPXCDOPsRwzh71zrCVkcT4SfwxBlQeHwMbH1aTGcSdI00GRLaH6iXRSBTazGH/u6sGQTc1tGqofg=="
          crossorigin="anonymous" referrerpolicy="no-referrer" />
    <link rel="stylesheet"
          href="https://cdnjs.cloudflare.com/ajax/libs/reveal.js/4.1.2/plugin/highlight/zenburn.min.css"
          integrity="sha512-JPxjD2t82edI35nXydY/erE9jVPpqxEJ++6nYEoZEpX2TRsmp2FpZuQqZa+wBCen5U16QZOkMadGXHCfp+tUdg=="
          crossorigin="anonymous" referrerpolicy="no-referrer" />
    <script src="https://cdnjs.cloudflare.com/ajax/libs/reveal.js/4.1.2/reveal.min.js"
            integrity="sha512-K7P1+dtPriNNHlE4aJr+JKx1X6R0wvy24QBqL2CxaHc4XdkQjrH2t2FCrgoxZGMh6s1TgigNLEdrWa6NJra6Zg=="
            crossorigin="anonymous"
            referrerpolicy="no-referrer"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/reveal.js/4.1.2/plugin/markdown/markdown.min.js"
            integrity="sha512-VMIcHSU7cVYRxUYr9l/t/iRO98QUfuTw8inT0mFklWb6HhKwLkjA0F24O42RL39RKGenNEK/TlEWB2NEy8w4Ng=="
            crossorigin="anonymous"
            referrerpolicy="no-referrer"></script>

    <script src="https://cdnjs.cloudflare.com/ajax/libs/reveal.js/4.1.2/plugin/highlight/highlight.min.js"
            integrity="sha512-NVdqCvsrfhXUYzGTEGGBFf5jxheVVcglfdvO8r+WdG0BKMrRYvaiCGIC0S+wf4hkE3Xq43gTd1k3Mwl/UNgdvw=="
            crossorigin="anonymous"
            referrerpolicy="no-referrer"></script>

    <script src="https://cdnjs.cloudflare.com/ajax/libs/reveal.js/4.1.2/plugin/math/math.min.js"
            integrity="sha512-3HmB6PODGpVVNHOzPd/iva9nhAtO8o5lNh8ukjgcswV+iQTRZFqdMWJW25dvcSploiX4I4M6uuUKIV9BaogQhw=="
            crossorigin="anonymous" referrerpolicy="no-referrer"></script>
<style type="text/css">
  .container {
      display: flex;
  }
  .col {
      flex: 1;
  }
  .reveal p {
      text-align: left;
  }
  .reveal .center {
      margin-left: auto;
      margin-right: auto;
      display: inline-block;
  }
  .reveal ul {
      display: block;
  }
  .reveal ol {
      display: block;
  }
  .reveal img {
      margin-left: auto;
      margin-right: auto;
      display: block;
  }
</style>
</head>
""".strip()

footer = """
<script>
  function extend() {
    var target = {};
    for (var i = 0; i < arguments.length; i++) {
      var source = arguments[i];
      for (var key in source) {
        if (source.hasOwnProperty(key)) {
          target[key] = source[key];
        }
      }
    }
    return target;
  }

  // default options to init reveal.js
  var defaultOptions = {
    controls: true,
    progress: true,
    history: true,
    slideNumber: true,
    center: true,
    transition: 'none', // none/fade/slide/convex/concave/zoom
    plugins: [
      RevealMarkdown,
      RevealHighlight,
      RevealMath,
    ],
    markdown: {smartypants: true}
  };

  // options from URL query string
  var queryOptions = Reveal().getQueryHash() || {};

  var options = extend(defaultOptions, {}, queryOptions);
</script>


<script>
  Reveal.initialize(options);
</script>
""".strip()


def preprocess(inp):
    with open(inp, "r") as f:
        slides = f.read().strip()
    title = None
    if slides.startswith("---\n"):
        slides = slides[4:]
        end = slides.find("---\n")
        if end < 0:
            raise RuntimeError
        header = yaml.load(slides[:end], Loader=yaml.SafeLoader)
        title = header.get("title")
        slides = slides[end + 4 :]

    if title is None:
        print("warning: No title found")
        title = ""
    else:
        title = f"<title>{title}</title>"

    slides = slides.split("\n------\n")

    return (title, slides)


def generate(inp, outp, watch=False):
    if watch:
        intime = os.stat(inp).st_mtime
        try:
            outtime = os.stat(outp).st_mtime
        except FileNotFoundError:
            outtime = float("-inf")
        if intime < outtime:
            return
    title, slides = preprocess(inp)
    lines = [header % title, "<body>", '<div class="reveal">', '<div class="slides">']
    for slide in slides:
        lines.append('<section>')
        sections = slide.strip().split("\n--->\n")
        for section in sections:
            lines.append('<section data-markdown><textarea data-template>')
            lines.append(f"\n{section.strip()}\n")
            lines.append("</textarea></section>")
        lines.append("</section>")
    lines.append("</div>")
    lines.append("</div>")
    lines.append(footer)
    lines.append("</body>")
    lines.append("</html>")
    with open(outp, "w") as f:
        f.write("\n".join(lines))


parser = argparse.ArgumentParser()
parser.add_argument("input")
parser.add_argument("--watch", action="store_true", default=False)
args = parser.parse_args()
inp = args.input
if not os.path.exists(inp):
    print("Input does not exist\n")
    sys.exit(1)

if os.path.isfile(inp):
    infiles = [inp]
else:
    infiles = list(pathlib.Path(inp).glob("*.md"))

outfiles = []
for inp in infiles:
    outp, _ = os.path.splitext(os.path.basename(inp))
    outfiles.append(f"{outp}.html")

if args.watch:
    print("Watching for changes, hit C-c to stop")
while True:
    for inp, outp in zip(infiles, outfiles):
        generate(inp, outp, watch=args.watch)
    if args.watch:
        time.sleep(0.25)
    else:
        break
