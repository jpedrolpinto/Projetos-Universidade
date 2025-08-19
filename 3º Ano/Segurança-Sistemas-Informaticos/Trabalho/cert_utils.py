from cryptography.hazmat.primitives.serialization import pkcs12
from cryptography.x509.oid import NameOID

from S8_a104270.Client_sts import private_key


def extract_certificate_info(p12_filename):
    """
    Extrai informaçoes do certificado X.509 de um arquivo PKCS12
    :param p12_filename: Caminho para o arquivo PKCS12(.p12)
    :return: Um dicionário com as informaçoes extraidas do certificado
    """

    #ler o arquivo PKCS12
    with open(p12_filename, "rb") as f:
        p12_data = f.read()

    #extrair a chave privada, o certificado e os certificados CA
    private_key, certificate, ca_certs = pkcs12.load_key_and_certificates(p12_data, None)

    #extrair informaçoes do campo Subject
    subject_info = []
    for attr in certificate.subject:
        subject_info[attr.oid._name] = attr.value

    #extrair user_id do campo PSEUDONYM
    user_id = subject_info.get('pseudonym')