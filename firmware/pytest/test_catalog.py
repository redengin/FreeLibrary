import requests

catalog_base_url = "http://192.168.4.1/api/catalog/"
FILE_NAME = "testfile"
FILE_SIZE = 100000
FILE_PAYLOAD = "X" * FILE_SIZE
FILE_TITLE = "this is a testfile"
FILE_ICON = "X" * FILE_SIZE


def test_catalog():
    # create a new file
    test_url = catalog_base_url + FILE_NAME
    response = requests.put(test_url, data=FILE_PAYLOAD)
    assert response.status_code == 200

    # retrieve that file
    response = requests.get(test_url)
    assert 200 == response.status_code
    assert FILE_SIZE == len(response.content)
    assert FILE_PAYLOAD == response.content

    # remove that file
    response = requests.delete(test_url)
    assert response.status_code == 200





