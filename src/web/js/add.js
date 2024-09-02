$(document).ready(function () {
    $("#uploadForm").submit(function (event) {
        event.preventDefault();

        const formData = new FormData();
        const files = $("#formDirectory")[0].files;

        if (files.length === 0) {
            toastr.error('Не выбрана директория с файлами!');
            return;
        }

        for (const file of files) {
            formData.append("files", file);
        }

        $.ajax({
            type: "POST",
            url: `${location.protocol}//${location.host}/add`,
            data: formData,
            cache: false,
            contentType: false,
            processData: false,
            success: function (response) {
                handleSuccessResponse(response);
            },
            error: function (response) {
                handleErrorResponse(response);
            }
        });
    });

    function handleSuccessResponse(response) {
        $("#formDirectory")[0].value = "";
        const message = response.message || "Все файлы успешно загружены!";
        toastr.success(message);
    }

    function handleErrorResponse(response) {
        $("#formDirectory")[0].value = "";
        const message = response.responseJSON?.message || "Ошибка загрузки файлов!";
        toastr.error(message);
    }
});
