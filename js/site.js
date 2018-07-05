$(document).ready(function() {
	$('.sample-img').hover(function() {
		$(this).addClass('zoomed');
	}, function() {
		$(this).removeClass('zoomed');
	});

	$('[data-toggle="popover"]').popover({
		placement : 'top'
	});

	$('.btn').on('click', function (e) {
		$('.btn').not(this).popover('hide');
	});
});

$(document).on('click', '[data-toggle="lightbox"]', function(event) {
	event.preventDefault();
	$(this).ekkoLightbox();
});
