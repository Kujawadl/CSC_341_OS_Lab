function lineWrap(){
    var wrap = function () {
        var elems = document.getElementsByClassName('syntaxhighlighter');
        for (var j = 0; j < elems.length; ++j) {
            var sh = elems[j];
            var gLines = sh.getElementsByClassName('gutter')[0].getElementsByClassName('line');
            var cLines = sh.getElementsByClassName('code')[0].getElementsByClassName('line');
            var stand = 15;
            for (var i = 0; i < gLines.length; ++i) {
                var h = $(cLines[i]).height();
                if (h != stand) {
                    console.log(i);
                    gLines[i].setAttribute('style', 'height: ' + h + 'px !important;');
                }
            }
        }
     };
    var whenReady = function () {
        if ($('.syntaxhighlighter').length === 0) {
            setTimeout(whenReady, 800);
        } else {
            wrap();
        }
    };
    whenReady();
};
lineWrap();
$(window).resize(function(){lineWrap()});
