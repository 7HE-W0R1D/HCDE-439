function openLightbox(id="lightbox1") {
    document.getElementById(id).style.display = 'block';
    document.body.style.overflowY = "hidden";
}
  
function closeLightbox(id="lightbox1") {
    document.getElementById(id).style.display = 'none';
    document.body.style.overflowY = "auto";
}