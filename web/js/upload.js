$("#formFile").change(() => {
	for (let file of $("#formFile")[0].files) {
		var formData = new FormData();
		formData.append("file", file);
		$.ajax({
			type: "POST",
			url: location.protocol + "//" + location.host + "/add_file",
			cache: false,
			contentType: false,
			processData: false,
			data: formData,
			dataType: "json",
			success: function (msg) {
				$("#formFile")[0].value = "";
				if ("message" in msg) {
					toastr.success(msg.message, "Успех!");
				} else {
					toastr.error("Ошибка!");
				}
				$("#formFile").val([])
			},
			error: (msg) => {
				if ("message" in msg) {
					toastr.success(msg.message, "Успех!");
				} else {
					toastr.error("Ошибка!");
				}
				$("#formFile").val([])
			}
		});
	}
});