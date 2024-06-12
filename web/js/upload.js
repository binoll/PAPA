$(document).ready(function () {
    $("#formFile").change(() => {
        const files = $("#formFile")[0].files;

        for (const file of files) {
            const formData = new FormData();
            formData.append("file", file);

            $.ajax({
                type: "POST",
                url: `${location.protocol}//${location.host}/add_file`,
                cache: false,
                contentType: false,
                processData: false,
                data: formData,
                dataType: "json",
                success: function (response) {
                    handleSuccessResponse(response);
                },
                error: function (response) {
                    handleErrorResponse(response);
                }
            });
        }
    });

    function handleSuccessResponse(response) {
        $("#formFile")[0].value = "";
        const message = response.message || "Успех!";
        toastr.success(message);
        $("#formFile").val([]);
    }

    function handleErrorResponse(response) {
        $("#formFile")[0].value = "";
        const message = response.responseJSON?.message || "Ошибка!";
        toastr.error(message);
        $("#formFile").val([]);
    }
});
