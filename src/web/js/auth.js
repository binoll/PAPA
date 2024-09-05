$(document).ready(function () {
    $("form[action='/login']").submit(function (event) {
        event.preventDefault();

        if (!window.FormData) {
            toastr.error("Ваш браузер не поддерживает загрузку файлов через FormData.");
            return;
        }

        const formData = new FormData(this);

        $.ajax({
            type: "POST",
            url: `${location.protocol}//${location.host}/login`,
            data: formData,
            cache: false,
            contentType: false,
            processData: false,
            success: function (response) {
                window.location.href = '/';
            },
            error: function (response) {
                handleErrorResponse(response);
            }
        });
    });

    $("form[action='/register']").submit(function (event) {
        event.preventDefault();

        if (!window.FormData) {
            toastr.error("Ваш браузер не поддерживает загрузку файлов через FormData.");
            return;
        }

        const formData = new FormData(this);

        $.ajax({
            type: "POST",
            url: `${location.protocol}//${location.host}/register`,
            data: formData,
            cache: false,
            contentType: false,
            processData: false,
            success: function (response) {
                window.location.href = '/login';
            },
            error: function (response) {
                handleErrorResponse(response);
            }
        });
    });

    function handleErrorResponse(response) {
        const message = response.responseJSON?.message || "Ошибка выполнения операции!";
        toastr.error(message);
    }
});
