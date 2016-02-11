var release = 'Lab_1'
var url_base = 'https://raw.githubusercontent.com/Kujawadl/CSC_341_OS_Lab/' + release + '/'

var files = [
  {
    "anchor" : "makefile",
    "url" : url_base + 'makefile'
  },
  {
    "anchor" : "out_txt",
    "url" : url_base + 'out.txt'
  },
  {
    "anchor" : "machine_h",
    "url" : url_base + 'machine.h'
  },
  {
    "anchor" : "machine_c",
    "url" : url_base + 'machine.c'
  },
  {
    "anchor" : "os_h",
    "url" : url_base + 'os.h'
  },
  {
    "anchor" : "os_c",
    "url" : url_base + 'os.c'
  },
  {
    "anchor" : "part1_dat",
    "url" : url_base + 'part1.dat'
  },
  {
    "anchor" : "part1x_dat",
    "url" : url_base + 'part1x.dat'
  }
];

document.ready(function() {
  document.write('<h2><a id="project-listing" class="anchor" href="#project-listing" aria-hidden="true"><span aria-hidden="true" class="octicon octicon-link"></span></a>Project Listing</h2>');
  for (current in files) {
    document.write('<h3><a id=\"' + current.anchor + '\" class="anchor" href=\"#' + current.anchor + '\" aria-hidden="true"><span aria-hidden="true" class="octicon octicon-link"></span></a>machine.h</h3><pre id=\"' + current.anchor + '\"_code class="syntaxhighlighter brush: c; toolbar: false; tab-size: 2; ruler: false"></pre>');
    document.write('<p class=\"pagebreak\" />');

    $(current.anchor).load(current.url);
  }
});
