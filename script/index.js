
function scrollToTop() {
    window.scrollTo({top: 0, behavior: 'smooth'});
}

$(document).ready(function() {

    let topButton = document.getElementById("topBtn");

    window.onscroll = function() {scrollFunction()};

    function scrollFunction() {
        if (document.body.scrollTop > 20 || document.documentElement.scrollTop > 20) {
            topButton.classList.add("active");
        } else {
            topButton.classList.remove("active");
        }
    }

    scrollFunction();

    $(".nav-list [href]").each(function() {
        if (this.href == window.location.href) {
            $(this).addClass("active");
        }
        else {
            $(this).removeClass("active");
        }
    });
});

$(document).ready(function() {
	$(".menu-trigger").click(function() {
      console.log('clicked');
		$(".nav-list").slideToggle(400, function() {
			$(this).toggleClass("nav-expanded").css('display', '');
		});
	});


    $("li").click(function () {
        $('li > ul').not($(this).children("ul").slideToggle()).hide();        
    });
});