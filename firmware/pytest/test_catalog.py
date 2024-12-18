import requests

catalog_base_url = "http://192.168.4.1/api/catalog/"
FILE_NAME = "test/testfile"
FILE_SIZE = 100000
FILE_PAYLOAD = b"X" * FILE_SIZE
FILE_TITLE = "this is a testfile"
FILE_ICON = "X" * FILE_SIZE


def test_catalog():
    # create a new file
    test_url = catalog_base_url + FILE_NAME
    response = requests.put(test_url, data=FILE_PAYLOAD)
    assert 200 == response.status_code

    # retrieve that file
    response = requests.get(test_url)
    assert 200 == response.status_code
    assert FILE_SIZE == len(response.content)
    assert FILE_PAYLOAD == response.content

    # remove that file
    response = requests.delete(test_url)
    assert 200 == response.status_code


def test_catalog_timestamp():
    # create a new file with timestamp
    TIMESTAMP = "2025-01-06T01:02:03Z"
    test_url = catalog_base_url + FILE_NAME
    response = requests.put(test_url, query={"timestamp":TIMESTAMP}, data=FILE_PAYLOAD)
    assert 200 == response.status_code

    # retrieve the file
    response = requests.get(test_url)
    assert 200 == response.status_code
    assert TIMESTAMP == response.headers["X-FileTimeStamp"]

    # remove test file
    response = requests.delete(test_url)




