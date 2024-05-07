$("#formFile").change(() => {
  for (let file of $("#formFile")[0].files) {
    var formData = new FormData();
    formData.append("file", file);
    $.ajax({
      type: "POST",
      url: location.protocol + "//" + location.host + "/api/add_file",
      cache: false,
      contentType: false,
      processData: false,
      data: formData,
      dataType: "json",
      success: function (msg) {
        $("#formFile")[0].value = "";
        if ("message" in msg) {

          toastr.success(msg.message, file.name);
        } else {
          toastr.error("Что-то пошло не так...");
        }

        $("#formFile").val([])
      },
      error: (msg) => {

        if ("message" in msg) {

          toastr.success(msg.message, file.name);
        } else {
          toastr.error("Что-то пошло не так...");
        }

        $("#formFile").val([])
      }
    });
  }
});
